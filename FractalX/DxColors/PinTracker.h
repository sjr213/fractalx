#pragma once
#include <vector>
#include "ColorPin.h"
#include <atltypes.h>

namespace DxColor
{
	class CPinTracker
	{
	public:
		CPinTracker(CSize screenSize, int numberOfColors, int iconDimension, const std::vector<ColorPin>& pins, CPoint topLeft);
		~CPinTracker();

		void SetPins(const std::vector<ColorPin>& pins);

		int GetNumberOfPins() const;

		// -1 is not found
		int GetIndex(const CPoint& pt) const;

		void Move(int index, int deltaX, int deltaY);

		std::vector<ColorPin> GetPins() const;

		CPoint GetTopLeft(int index) const;

	protected:
		void SetPins();
		void SetTopLefts();
		void SetRects();

		// window to display
		double GetAdjustedPositionX() const;

		// display to window
		double GetUnAdjustedPositionX() const;

	private:
		CSize m_screenSize;
		int m_numberOfColors;
		int m_iconDim;
		CPoint m_topLeftDisplay;			// Display relative to dialog window

		std::vector<ColorPin> m_pins;
		std::vector<CRect> m_rects;
		std::vector<CPoint> m_topLefts;		// Relative to pin display
	};
}
