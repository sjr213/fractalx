#pragma once
#include <vector>
#include "ColorPin.h"
#include <atltypes.h>

namespace DxColor
{
	class CPinTracker
	{
	public:
		static constexpr double MaxIndex = 1.0;
		static constexpr int MaxIntIndex = 1000;	// not used by pin tracker yet since it used screensize.cx which is 1000 but should be updated

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

		// returns new pin index or -1 if failed
		int AddPin(const CPoint& pt, const std::shared_ptr<DxColor::ColorPin>& pPin);

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

		// returns new pin index or -1 if failed
		int AddPinBetween(int leftPin, int rightPin, const CPoint& pt, const std::shared_ptr<DxColor::ColorPin>& pPin);
		int AddPinRight(int leftPin, const CPoint& pt, const std::shared_ptr<DxColor::ColorPin>& pPin);
		int AddPinLeft(int rightIndex, const CPoint& pt, const std::shared_ptr<DxColor::ColorPin>& pPin);
		int AddFirstPin(const CPoint& pt, const std::shared_ptr<DxColor::ColorPin>& pPin);

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
