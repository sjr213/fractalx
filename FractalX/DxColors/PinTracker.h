#pragma once
#include <vector>
#include "ColorPin.h"
#include <atltypes.h>

namespace DxColor
{
	class CPinTracker
	{
	public:
		CPinTracker(CSize screenSize, int numberOfColors, int iconDimension, const std::vector<ColorPin>& pins);
		~CPinTracker();

		void SetPins(const std::vector<ColorPin>& pins);

		int GetNumberOfPins() const;

		// -1 is not found
		int GetIndex(const CPoint& pt) const;

		void Move(int index, int deltaX);

		std::vector<ColorPin> GetPins() const;

		int GetLeft(int index) const;

	protected:
		void SetPins();
		void SetLefts();
		void SetRects();

		double GetAdjustedPositionX() const;

	private:
		CSize m_screenSize;
		int m_numberOfColors;
		int m_iconDim;

		std::vector<ColorPin> m_pins;
		std::vector<CRect> m_rects;
		std::vector<int> m_lefts;
	};
}
