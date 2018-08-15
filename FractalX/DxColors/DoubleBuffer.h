#pragma once
#include <memory>
//#include <atltypes.h>

class CDC;
class CSize;
class CPoint;

// Draw size can be different than the display or screen size
// Display size should be the size drawn in the window
// The draw CDC will be drawn into the display CDC, and stretched to fill it

namespace DxColor
{
	class CDoubleBuffer
	{
	public:
		static std::shared_ptr<CDoubleBuffer> CreateBuffer();

		~CDoubleBuffer() {}

		virtual void SetDrawSize(CSize drawSize) = 0;

		virtual void SetDisplaySize(CSize displaySize) = 0;

		virtual void PrepDCs(CDC& dc) = 0;

		virtual CSize GetDrawSize() = 0;

		virtual CSize GetDisplaySize() = 0;

		// Only call just before Draw
		virtual CDC* GetDrawingDC() = 0;

		// Only call just before Draw
		virtual CDC* GetDisplayDC() = 0;

		virtual void Draw(CDC& dc, const CPoint& topLeft) = 0;

	protected:
		CDoubleBuffer() {}
	};
}