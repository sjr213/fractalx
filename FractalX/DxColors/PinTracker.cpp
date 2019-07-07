#include "stdafx.h"
#include "PinTracker.h"

#include <algorithm>
#include <assert.h>
#include "DxColorUtilities.h"
#include <optional>

using namespace std;

namespace DxColor
{
	CPinTracker::CPinTracker(CSize screenSize, int numberOfColors, int iconDimension, const vector<ColorPin>& pins, CPoint topLeft)
		: m_screenSize(screenSize)
		, m_numberOfColors(numberOfColors)
		, m_iconDim(iconDimension)
		, m_pins(pins)
		, m_topLeftDisplay(topLeft)
	{
		SetPins();
	}

	CPinTracker::~CPinTracker()
	{
	}

	void CPinTracker::SetPins(const vector<ColorPin>& pins)
	{
		m_pins = pins;

		SetPins();
	}

	int CPinTracker::GetNumberOfPins() const
	{
		return static_cast<int>(m_pins.size());
	}

	int CPinTracker::GetIndex(const CPoint & pt) const
	{
		int nPins = static_cast<int>(m_rects.size());

		for (int i = 0; i < nPins; ++i)
		{
			if (m_rects.at(i).PtInRect(pt))
				return i;
		}

		return -1;
	}

	// check this later
	int CPinTracker::Move(int index, int deltaX)
	{
		if (index < 0 || index >= static_cast<int>(m_pins.size()))
			return -1;

		CRect rect = m_rects.at(index);

		rect.left += deltaX;
		if (rect.left < m_topLeftDisplay.x - m_iconDim / 2)
			rect.left = m_topLeftDisplay.x - m_iconDim / 2;
		if (rect.left > m_topLeftDisplay.x + m_screenSize.cx - m_iconDim / 2)
			rect.left = m_topLeftDisplay.x + m_screenSize.cx - m_iconDim / 2;

		rect.right = rect.left + m_iconDim;
		m_rects.at(index) = rect;

		// calculate top lefts relative to display
		int left = static_cast<int>(GetAdjustedPositionX() * (rect.left - m_topLeftDisplay.x));

		m_topLefts.at(index).x = left;

		double center = left + m_iconDim / 2.0;
		double newPosition = center / m_screenSize.cx;

		newPosition = std::max(0.0, std::min(newPosition, MaxIndex));

		m_pins.at(index).Index = newPosition;

		return static_cast<int>(m_numberOfColors * newPosition);
	}

	vector<ColorPin> CPinTracker::GetPins() const
	{
		return m_pins;
	}

	CPoint CPinTracker::GetTopLeft(int index) const
	{
		return m_topLefts.at(index);
	}

	double CPinTracker::GetAdjustedPositionX() const
	{
		return static_cast<double>(m_screenSize.cx) / m_numberOfColors;
	}

	// display to window
	double CPinTracker::GetUnAdjustedPositionX() const 
	{
		return static_cast<double>(m_numberOfColors) / m_screenSize.cx;
	}

	void CPinTracker::SetPins()
	{
		SetTopLefts();
		SetRects();
	}

	void CPinTracker::SetTopLefts()
	{
		int nPins = static_cast<int>(m_pins.size());
		m_topLefts.resize(nPins);

		double adjustedPos = GetAdjustedPositionX();
		double halfIconWidth = 0.5 * m_iconDim *  adjustedPos;

		// The 2 is so the pin doesn't touch the bottom
		int top = m_screenSize.cy - m_iconDim - 2;

		for (int i = 0; i < nPins; ++i)
		{
			double centerPin = m_pins.at(i).Index * adjustedPos * m_numberOfColors;
			CPoint topLeft(static_cast<int>(centerPin - halfIconWidth + 0.5), top);
			m_topLefts.at(i) = topLeft;
		}
	}

	void CPinTracker::SetRects()
	{
		int nPins = static_cast<int>(m_pins.size());
		m_rects.resize(nPins);

		double unAdjust = GetUnAdjustedPositionX();

		for (int i = 0; i < nPins; ++i)
		{
			CPoint topLeft = m_topLefts.at(i);

			int left = static_cast<int>(topLeft.x * unAdjust) + m_topLeftDisplay.x;
			int top = topLeft.y + m_topLeftDisplay.y;

			m_rects.at(i) = CRect(left, top, left + m_iconDim, top + m_iconDim);
		}
	}

	static optional<int> GetLeftPin(const vector<CRect>& rects, const CPoint& pt)
	{
		int nPins = static_cast<int>(rects.size());

		for (int i = nPins - 1; i >= 0; --i)
		{
			if (rects.at(i).right < pt.x)
				return i;
		}

		return nullopt;
	}

	static optional<int> GetRightPin(const vector<CRect>& rects, const CPoint& pt)
	{
		int nPins = static_cast<int>(rects.size());

		for (int i = 0; i < nPins; ++i)
		{
			if (rects.at(i).left > pt.x)
				return i;
		}

		return nullopt;
	}

	int CPinTracker::AddPinBetween(int leftIndex, int rightIndex, const CPoint& pt, const std::shared_ptr<DxColor::ColorPin>& pPin)
	{
		const auto& leftPin = m_pins.at(leftIndex);
		const auto& rightPin = m_pins.at(rightIndex);

		// try averaging colors
		ColorPin newPin = pPin!= nullptr ? *pPin: leftPin;

		const CRect& leftRect = m_rects.at(leftIndex);
		const CRect& rightRect = m_rects.at(rightIndex);

		CPoint leftCenter = leftRect.CenterPoint();
		CPoint rightCenter = rightRect.CenterPoint();

		double mult = static_cast<double>(pt.x - leftCenter.x) / (rightCenter.x - leftCenter.x);
		newPin.Index = leftPin.Index + mult * (rightPin.Index - leftPin.Index);

		if (!pPin)
		{
			newPin.Color1 = DxColor::Utilities::AverageColors(leftPin.Color1, rightPin.Color1, mult);
			newPin.Color2 = DxColor::Utilities::AverageColors(leftPin.Color2, rightPin.Color2, mult);
		}

		m_pins.insert(begin(m_pins) + rightIndex, newPin);

		SetPins();

		return rightIndex;
	}

	int CPinTracker::AddPinRight(int leftIndex, const CPoint& pt, const std::shared_ptr<DxColor::ColorPin>& pPin)
	{
		const auto& leftPin = m_pins.at(leftIndex);
		const CRect& leftRect = m_rects.at(leftIndex);
		CPoint leftCenter = leftRect.CenterPoint();

		assert(leftCenter.x < m_screenSize.cx);

		double mult = static_cast<double>(pt.x - leftCenter.x) / (m_screenSize.cx - leftCenter.x);
		double newIndex = leftPin.Index + mult * (MaxIndex - leftPin.Index);
		newIndex = std::max(leftPin.Index + 0.001, newIndex);
		newIndex = std::min(newIndex, MaxIndex);

		ColorPin newPin = pPin != nullptr ? *pPin : leftPin;
		newPin.Index = newIndex;
	
		m_pins.push_back(newPin);

		SetPins();

		return static_cast<int>(m_pins.size()-1);
	}

	int CPinTracker::AddPinLeft(int rightIndex, const CPoint& pt, const std::shared_ptr<DxColor::ColorPin>& pPin)
	{
		const auto& rightPin = m_pins.at(rightIndex);
		const CRect& rightRect = m_rects.at(rightIndex);
		CPoint rightCenter = rightRect.CenterPoint();

		assert(pt.x >= 0);

		double mult = static_cast<double>(pt.x) / rightCenter.x;
		double newIndex = mult * rightPin.Index;
		newIndex = std::max(0.0, newIndex);
		newIndex = std::min(newIndex, MaxIndex);

		ColorPin newPin = pPin != nullptr ? *pPin : rightPin;
		newPin.Index = newIndex;

		m_pins.insert(begin(m_pins) + rightIndex, newPin);

		SetPins();

		return rightIndex;
	}

	int CPinTracker::AddFirstPin(const CPoint& pt, const std::shared_ptr<DxColor::ColorPin>& pPin)
	{
		if (pt.x < 0)
			return false;

		if (pt.x > m_screenSize.cx)
			return false;

		ColorPin newPin = pPin != nullptr ? *pPin : ColorPin();;
		double newIndex = static_cast<double>(pt.x) / m_screenSize.cx;
		newIndex = std::max(0.0, newIndex);
		newIndex = std::min(newIndex, MaxIndex);
		newPin.Index = newIndex;

		m_pins.insert(begin(m_pins), newPin);

		SetPins();

		return 0;
	}

	// returns false if pin is too close to another
	int CPinTracker::AddPin(const CPoint& pt, const std::shared_ptr<DxColor::ColorPin>& pPin)
	{
		if (GetIndex(pt) >= 0)
			return -1;

		optional<int> leftPinIndex = GetLeftPin(m_rects, pt);
		optional<int> rightPinIndex = GetRightPin(m_rects, pt);

		// If left and right - AddPinBetween
		if (leftPinIndex.has_value() && rightPinIndex.has_value())
			return AddPinBetween(leftPinIndex.value(), rightPinIndex.value(), pt, pPin);

		if (leftPinIndex.has_value())
			return AddPinRight(leftPinIndex.value(), pt, pPin);

		if (rightPinIndex.has_value())
			return AddPinLeft(rightPinIndex.value(), pt, pPin);

		return AddFirstPin(pt, pPin);
	}

	bool CPinTracker::SpreadPins()
	{
		int nPins = static_cast<int>(m_pins.size());

		if (nPins < 2)
			return false;

		double spacing = MaxIndex / (nPins - 1);

		sort(begin(m_pins), end(m_pins), [&](const ColorPin& lf, const ColorPin& rt) { return lf.Index < rt.Index; });

		for (int i = 0; i < nPins; ++i)
		{
			double index = clamp(i * spacing, 0.0, MaxIndex);
			m_pins.at(i).Index = index;
		}

		SetPins();

		return true;
	}
}