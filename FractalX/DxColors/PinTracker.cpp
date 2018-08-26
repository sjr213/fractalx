#include "stdafx.h"
#include "PinTracker.h"
#include <algorithm>

namespace DxColor
{
	CPinTracker::CPinTracker(CSize screenSize, int numberOfColors, int iconDimension, const std::vector<ColorPin>& pins, CPoint topLeft)
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

	void CPinTracker::SetPins(const std::vector<ColorPin>& pins)
	{
		m_pins = pins;
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
	void CPinTracker::Move(int index, int deltaX)
	{
		if (index < 0 || index >= static_cast<int>(m_pins.size()))
		{
			return;
		}

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

		newPosition = std::max(0.0, std::min(newPosition, 1.0));

		m_pins.at(index).Index = newPosition;
	}

	std::vector<ColorPin> CPinTracker::GetPins() const
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
}