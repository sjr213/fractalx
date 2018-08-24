#include "stdafx.h"
#include "DoubleBuffer.h"
#include <afxwin.h>

// To use
// Create a CDoubleBufferImpl
// Call SetDrawSize() this is the size of the background image that will be custom drawn (palette color bars)
// Call SetDisplaySize() this is the size on the screen the whole image will be displayed in
// Call PrepDCs() after the sizes are final
// When ready to draw -
//		1) Call GetDrawingDC() to get the DC to draw the custom background image
//		2) Call GetDisplayDC() to get the DC for the screen, the drawing DC will already be drawn on this
//			This can be called again without redrawing on the drawing DC
//		3) Call Draw() to draw to the screen window
namespace DxColor
{
	const COLORREF BACK_COLOR = RGB(225, 225, 225);

	class CDoubleBufferImpl : public CDoubleBuffer
	{
	public:
		CDoubleBufferImpl()
		{}

		~CDoubleBufferImpl() {}

		void SetDrawSize(CSize drawSize) override
		{
			if (m_drawSize == drawSize)
				return;

			m_drawBitmap.reset();
			m_drawDC.reset();
			m_drawSize = drawSize;
		}

		void SetDisplaySize(CSize displaySize) override
		{
			if (m_displaySize == displaySize)
				return;

			m_displayBitmap.reset();
			m_displayDC.reset();
			m_displaySize = displaySize;
		}

		void PrepDCs(CDC& dc) override
		{
			if (m_drawDC->GetSafeHdc() == NULL)
			{
				m_drawDC.reset(new CDC);
				m_drawBitmap.reset(new CBitmap);
			}

			if (m_displayDC->GetSafeHdc() == NULL)
			{
				m_displayDC.reset(new CDC);
				m_displayBitmap.reset(new CBitmap);
			}

			m_drawDC->CreateCompatibleDC(&dc);
			OnPrepareDC(m_drawDC.get());

			if (!m_drawBitmap->CreateCompatibleBitmap(&dc, m_drawSize.cx, m_drawSize.cy))
				throw std::exception("Could not create draw bitmap");

			m_displayDC->CreateCompatibleDC(&dc);
			OnPrepareDC(m_displayDC.get());

			if (!m_displayBitmap->CreateCompatibleBitmap(&dc, m_displaySize.cx, m_displaySize.cy))
				throw std::exception("Could not create display bitmap");
		}

		CSize GetDrawSize() override
		{
			return m_drawSize;
		}

		CSize GetDisplaySize() override
		{
			return m_displaySize;
		}

		CDC* GetDrawingDC() override
		{
			if(!m_oldDrawBitmap)
				m_oldDrawBitmap = m_drawDC->SelectObject(m_drawBitmap.get());

			CRect backRect(0, 0, m_drawSize.cx, m_drawSize.cy);

			CBrush backBrush(BACK_COLOR);
			m_drawDC->FillRect(&backRect, &backBrush);

			return m_drawDC.get();
		}

		CDC* GetDisplayDC() override
		{
			if (!m_oldDrawBitmap)
				m_oldDrawBitmap = m_drawDC->SelectObject(m_drawBitmap.get());

			if(! m_oldDisplayBitmap)
				m_oldDisplayBitmap = m_displayDC->SelectObject(m_displayBitmap.get());

			m_displayDC->SetStretchBltMode(COLORONCOLOR);

			// this may not be correct
			m_displayDC->StretchBlt(0, 0, m_displaySize.cx, m_displaySize.cy, m_drawDC.get(), 0, 0, m_drawSize.cx, m_drawSize.cy, SRCCOPY);

			return m_displayDC.get();
		}

		void Draw(CDC& dc, const CPoint& topLeft) override
		{
			// draw to screen
			dc.BitBlt(topLeft.x, topLeft.y, m_displaySize.cx, m_displaySize.cy,
				m_displayDC.get(), 0, 0, SRCCOPY);

			// copy the old bitmaps back
			m_displayDC->SelectObject(m_oldDisplayBitmap);
			m_oldDisplayBitmap = nullptr;
			m_drawDC->SelectObject(m_oldDrawBitmap);
			m_oldDrawBitmap = nullptr;
		}

	protected:
		void OnPrepareDC(CDC* pDC)
		{
			pDC->SetMapMode(MM_TEXT);
		}

	private:
		CSize m_drawSize = CSize(100, 100);

		std::shared_ptr<CBitmap> m_drawBitmap;

		std::shared_ptr<CDC> m_drawDC;

		CSize m_displaySize = CSize(100, 100);

		std::shared_ptr<CBitmap> m_displayBitmap;

		std::shared_ptr<CDC> m_displayDC;

		CBitmap *m_oldDrawBitmap = nullptr;

		CBitmap *m_oldDisplayBitmap = nullptr;
		
	};

	std::shared_ptr<CDoubleBuffer> CDoubleBuffer::CreateBuffer()
	{
		return std::make_shared<CDoubleBufferImpl>();
	}
}