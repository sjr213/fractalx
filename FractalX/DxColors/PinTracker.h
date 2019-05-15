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

		// returns the index of the moved pin
		int Move(int index, int deltaX);

		std::vector<ColorPin> GetPins() const;

		CPoint GetTopLeft(int index) const;

		// returns false if pin is too close to another
		bool AddPin(const CPoint& pt);

		// returns true if pins changed
		bool SpreadPins();

	protected:
		void SetPins();
		void SetTopLefts();
		void SetRects();

		// window to display
		double GetAdjustedPositionX() const;

		// display to window
		double GetUnAdjustedPositionX() const;

		// return true if successful and the pins should be redrawn
		bool AddPinBetween(int leftPin, int rightPin, const CPoint& pt);
		bool AddPinRight(int leftPin, const CPoint& pt);
		bool AddPinLeft(int rightIndex, const CPoint& pt);
		bool AddFirstPin(const CPoint& pt);

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
