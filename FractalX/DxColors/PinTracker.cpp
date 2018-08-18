#include "stdafx.h"
#include "PinTracker.h"

namespace DxColor
{
	CPinTracker::CPinTracker(CSize screenSize, int numberOfColors, int iconDimension, const std::vector<ColorPin>& pins)
		: m_screenSize(screenSize)
		, m_numberOfColors(numberOfColors)
		, m_iconDim(iconDimension)
		, m_pins(pins)
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

		CPoint ptCopy(pt);
		ptCopy.x = static_cast<int>(pt.x * GetAdjustedPositionX() + 0.5);

		for (int i = 0; i < nPins; ++i)
		{
			if (m_rects.at(i).PtInRect(ptCopy))
				return i;
		}

		return -1;
	}

	// check this later
	void CPinTracker::Move(int index, int deltaX)
	{
		if (index >= 0 && index < static_cast<int>(m_pins.size()))
		{
			return;
		}

		double adjustedPos = GetAdjustedPositionX();

		int correctedX = static_cast<int>(deltaX * adjustedPos + 0.5);
		m_lefts.at(index) += correctedX;

		// set rect	
		double iconDim = m_iconDim * adjustedPos;

		int top = static_cast<int>(m_screenSize.cy - iconDim - 2);
		int bottom = static_cast<int>(top + iconDim);
		int left = m_lefts.at(index);
		m_rects.at(index) = CRect(left, top, static_cast<int>(left + iconDim), bottom);

		double center = m_lefts.at(index) + iconDim / 2.0;
		double newPosition = center / m_screenSize.cx;
		m_pins.at(index).Index = newPosition;
	}

	std::vector<ColorPin> CPinTracker::GetPins() const
	{
		return m_pins;
	}

	int CPinTracker::GetLeft(int index) const
	{
		return m_lefts.at(index);
	}

	double CPinTracker::GetAdjustedPositionX() const
	{
		return static_cast<double>(m_screenSize.cx) / m_numberOfColors;
	}

	void CPinTracker::SetPins()
	{
		SetLefts();
		SetRects();
	}

	void CPinTracker::SetLefts()
	{
		int nPins = static_cast<int>(m_pins.size());
		m_lefts.resize(nPins);

		double adjustedPos = GetAdjustedPositionX();
		double halfIconWidth = 0.5 * m_iconDim *  adjustedPos;

		for (int i = 0; i < nPins; ++i)
		{
			double centerPin = m_pins.at(i).Index * adjustedPos * m_numberOfColors;
			m_lefts.at(i) = static_cast<int>(centerPin - halfIconWidth + 0.5);
		}
	}

	void CPinTracker::SetRects()
	{
		int nPins = static_cast<int>(m_pins.size());
		m_rects.resize(nPins);

		double adjustedPos = GetAdjustedPositionX();
		double iconDim = m_iconDim *  adjustedPos;

		int top = static_cast<int>(m_screenSize.cy - iconDim - 2);
		int bottom = static_cast<int>(top + iconDim);

		for (int i = 0; i < nPins; ++i)
		{
			int left = m_lefts.at(i);
			m_rects.at(i) = CRect(left, top, static_cast<int>(left + iconDim), bottom);
		}
	}
}