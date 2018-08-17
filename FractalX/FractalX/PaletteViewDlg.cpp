
#include "stdafx.h"
#include "PaletteViewDlg.h"
#include "Resource.h"
#include "Messages.h"
#include "DoubleBuffer.h"
#include "ColorUtilities.h"

using namespace DxColor;

class CPaletteViewDlgImp : public CPaletteViewDlg
{
private:
	PinPalette m_palette;
	CWnd* m_parent = nullptr;
	std::function<void(const PinPalette&)> m_newPaletteMethod;
	std::shared_ptr<CDoubleBuffer> m_doubleBuffer;
	CPoint m_topLeft = CPoint(0, 0);
	CRect m_paletteRect;
	bool m_dcNotReady = true;
	bool m_paletteNeedsDrawing = true;
	std::vector<uint32_t> m_colors;
	HICON m_hIcon;

	const int NumberOfColors = 1000;
	const int ColorLineHeight = 10;
	const int BorderWidth = 10;
	const int ControlRegionHeight = 50;

public:
	CPaletteViewDlgImp(const PinPalette& palette, CWnd* pParent)
		: CPaletteViewDlg(IDD_PALETTE_VIEW_DLG, pParent)
		, m_palette(palette)
		, m_parent(pParent)
		, m_doubleBuffer(CDoubleBuffer::CreateBuffer())
	{}

	virtual ~CPaletteViewDlgImp()
	{
		if (m_hIcon)
			DestroyIcon(m_hIcon);
	}

	void SetNewPaletteMethod(std::function<void(const PinPalette&)> newPaletteMethod) override
	{
		m_newPaletteMethod = newPaletteMethod;
	}

	// Dialog Data
	enum { IDD = IDD_PALETTE_VIEW_DLG };

protected:
	BOOL OnInitDialog() override
	{
		CPaletteViewDlg::OnInitDialog();

		// the Pin - we need to use the instance handle for the dll because that's where the resource (icon) is
		m_hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PIN_ICON), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);

		CRect clientRect;
		GetClientRect(clientRect);

		InitDoubleBuffer(clientRect);

		m_colors = fx::ColorUtilities::ConvertPalette(m_palette, NumberOfColors);

		return TRUE;
	}

	void InitDoubleBuffer(const CRect& clientRect)
	{
		CSize drawSize(NumberOfColors, ColorLineHeight);
		m_doubleBuffer->SetDrawSize(drawSize);

		CSize displaySize(clientRect.Size());
		if (displaySize.cx > 3 * BorderWidth)
		{
			displaySize.cx -= 2 * BorderWidth;
			m_topLeft.x = BorderWidth;
		}

		if (displaySize.cy > 2 * ControlRegionHeight)
		{
			displaySize.cy -= (2 * ControlRegionHeight);
			m_topLeft.y = ControlRegionHeight;
		}

		m_paletteRect = CRect(m_topLeft, displaySize);

		m_doubleBuffer->SetDisplaySize(displaySize);
	}

	void PaletteChanged()
	{
		m_colors = fx::ColorUtilities::ConvertPalette(m_palette, NumberOfColors);

		UpdateData(FALSE);
		InvalidateRect(m_paletteRect);
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CPaletteViewDlg::DoDataExchange(pDX);

		if (!pDX->m_bSaveAndValidate)
		{
			auto name = m_palette.Name;
			DDX_Text(pDX, IDC_PALETTE_NAME_EDIT, name);
		}
	}

	DECLARE_MESSAGE_MAP()

	afx_msg void OnOk()
	{
		if (!m_parent)
		{
			ASSERT(false);
			return;
		}

		if (m_newPaletteMethod)
			m_newPaletteMethod(m_palette);

		m_parent->PostMessage(cMessage::tm_killPaletteView, 0, 0);
	}

	afx_msg void OnBnClickedCancel()
	{
		if (!m_parent)
		{
			ASSERT(false);
			return;
		}

		m_parent->PostMessage(cMessage::tm_killPaletteView, 0, 0);
	}

	void PrepBuffer(CDC& dc)
	{
		if (m_dcNotReady)
		{
			m_doubleBuffer->PrepDCs(dc);
			m_dcNotReady = false;
		}	
	}

	void DrawPaletteColors(CDC& dc)
	{
		for (int x = 0; x < NumberOfColors; ++x)
		{
			auto uColor = m_colors.at(x);
			COLORREF color = fx::ColorUtilities::ToColorRef(uColor);
			CPen aPen;
			aPen.CreatePen(PS_SOLID, 1, color);

			CPen *pOldPen = (CPen*)dc.SelectObject(&aPen);

			dc.MoveTo(x, 0);
			dc.LineTo(x, ColorLineHeight);

			dc.SelectObject(pOldPen);
		}
	}

	void DrawPalatte(CDC& dc)
	{
		if (m_paletteNeedsDrawing)
		{
			CDC* pDC = m_doubleBuffer->GetDrawingDC();
			
			if(pDC)
				DrawPaletteColors(*pDC);

			m_paletteNeedsDrawing = false;
		}
	}

	void DrawPins()
	{
		auto pDC = m_doubleBuffer->GetDisplayDC();

		int top = m_doubleBuffer->GetDisplaySize().cy - 34;

		pDC->DrawIcon(0, top, m_hIcon);
	}

	void OnPaint()
	{
		CPaintDC dc(this); // device context for painting

		PrepBuffer(dc);
		DrawPalatte(dc);		
		DrawPins();

		m_doubleBuffer->Draw(dc, m_topLeft);
	}
};

BEGIN_MESSAGE_MAP(CPaletteViewDlgImp, CPaletteViewDlg)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CPaletteViewDlgImp::OnOk)
	ON_BN_CLICKED(IDCANCEL, &CPaletteViewDlgImp::OnBnClickedCancel)
END_MESSAGE_MAP()

std::shared_ptr<CPaletteViewDlg> CPaletteViewDlg::CreatePaletteViewDlg(const PinPalette& palette, CWnd* pParent)
{
	return std::make_shared <CPaletteViewDlgImp>(palette, pParent);
}