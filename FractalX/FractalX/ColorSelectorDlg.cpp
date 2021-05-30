#include "stdafx.h"
#include "ColorSelectorDlg.h"

#include "ColorContrast.h"
#include "ColorUtils.h"
#include <Gdiplus.h>
#include "hslUtilities.h"
#include "Resource.h"

using namespace DxColor;
using namespace ColorUtils;

class ColorSelectorDlgImpl : public CColorSelectorDlg
{
private:
	DxColor::ColorArgb m_rawColor;
	COLORREF m_rgb;
	HSL m_hsl;
	bool m_enableAlpha;

public:
	ColorSelectorDlgImpl(ColorArgb color, bool enableAlpha, CWnd* pParent)
		: CColorSelectorDlg(IDD_COLOR_DLG, pParent)
		, m_rawColor(color)
		, m_enableAlpha(enableAlpha)
	{
		m_rgb = ToColorRef(color);
		m_hsl = ToHsl(color);
	}

	virtual ~ColorSelectorDlgImpl() {}

	DxColor::ColorArgb GetColor() override
	{
		return m_rawColor;
	}

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override
	{
		CColorSelectorDlg::OnInitDialog();

		EnableCtrls();

		return TRUE;
	}

	void EnableCtrls()
	{
		CWnd* pWnd = GetDlgItem(IDC_ALPHA_EDIT);
		if (pWnd)
			pWnd->EnableWindow(m_enableAlpha);
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CColorSelectorDlg::DoDataExchange(pDX);

		int red = GetRValue(m_rgb);
		DDX_Text(pDX, IDC_RED_EDIT, red);
		DDV_MinMaxInt(pDX, red, 0, 255);

		int green = GetGValue(m_rgb);
		DDX_Text(pDX, IDC_GREEN_EDIT, green);
		DDV_MinMaxInt(pDX, red, 0, 255);

		int blue = GetBValue(m_rgb);
		DDX_Text(pDX, IDC_BLUE_EDIT, blue);
		DDV_MinMaxInt(pDX, blue, 0, 255);

		int alpha = m_rawColor.A;
		DDX_Text(pDX, IDC_ALPHA_EDIT, alpha);
		DDV_MinMaxInt(pDX, alpha, 0, 255);

		m_rgb = RGB(static_cast<BYTE>(red), static_cast<BYTE>(green), static_cast<BYTE>(blue));
		m_rawColor.A = static_cast<bite>(alpha);

		double hue = m_hsl.H;
		DDX_Text(pDX, IDC_HUE_EDIT, hue);
		DDV_MinMaxDouble(pDX, hue, 0.0, MAX_HUE);

		double sat = m_hsl.S;
		DDX_Text(pDX, IDC_SATURATION_EDIT, sat);
		DDV_MinMaxDouble(pDX, sat, 0.0, MAX_SATURATION);

		double light = m_hsl.L;
		DDX_Text(pDX, IDC_LIGHTNESS_EDIT, light);
		DDV_MinMaxDouble(pDX, light, 0.0, MAX_LIGHTNESS);

		m_hsl = HSL(hue, sat, light);
	}

	void OnKillFocusRGB()
	{
		UpdateData(TRUE);

		m_rawColor = ToArgb(m_rgb, m_rawColor.A);
		m_hsl = ToHsl(m_rawColor);

		UpdateData(FALSE);
		Invalidate(TRUE);
	}

	void OnKillFocusHsl()
	{
		UpdateData(TRUE);

		m_rawColor = ToRgb(m_hsl, m_rawColor.A);
		m_rgb = ToColorRef(m_rawColor);

		UpdateData(FALSE);
		Invalidate(TRUE);
	}

	void OnBnColorChooser()
	{
		CColorDialog dlg;
		dlg.m_cc.Flags |= CC_FULLOPEN;
		dlg.m_cc.Flags |= CC_RGBINIT;
		auto colorBottom1 = ToColorRef(m_rawColor);
		dlg.m_cc.lpCustColors = &colorBottom1;

		if (dlg.DoModal() == IDOK)
		{
			m_rawColor = ToArgb(dlg.GetColor(), m_rawColor.A);
			m_rgb = ToColorRef(m_rawColor);
			m_hsl = ToHsl(m_rawColor);

			UpdateData(FALSE);
			Invalidate(TRUE);
		}
	}

	void OnPaint()
	{
		using namespace Gdiplus;

		CPaintDC dc(this);
		Graphics graphics(dc);

		Gdiplus::Rect rect(19, 37, 40, 40);

#pragma warning(push)
#pragma warning(disable: 4245)
		HatchBrush backGroundBrush(Gdiplus::HatchStyle::HatchStyleWideDownwardDiagonal, Color::Black, Color::White);
#pragma warning(pop)

		graphics.FillRectangle(&backGroundBrush, rect);

		SolidBrush colorBrush(ConvertToGdiColor(m_rawColor));

		graphics.FillRectangle(&colorBrush, rect);

		// Create a Pen object.
		Pen blackPen(Color(255, 0, 0, 0), 2);	

		// Draw the rectangle.
		graphics.DrawRectangle(&blackPen, rect);
	}

};

BEGIN_MESSAGE_MAP(ColorSelectorDlgImpl, CColorSelectorDlg)
	ON_WM_PAINT()
	ON_EN_KILLFOCUS(IDC_RED_EDIT, &ColorSelectorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_GREEN_EDIT, &ColorSelectorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_BLUE_EDIT, &ColorSelectorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_ALPHA_EDIT, &ColorSelectorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_HUE_EDIT, &ColorSelectorDlgImpl::OnKillFocusHsl)
	ON_EN_KILLFOCUS(IDC_SATURATION_EDIT, &ColorSelectorDlgImpl::OnKillFocusHsl)
	ON_EN_KILLFOCUS(IDC_LIGHTNESS_EDIT, &ColorSelectorDlgImpl::OnKillFocusHsl)
	ON_BN_CLICKED(IDC_COLOR_CHOOSER_BUT, &ColorSelectorDlgImpl::OnBnColorChooser)
END_MESSAGE_MAP()

std::shared_ptr<CColorSelectorDlg> CColorSelectorDlg::CreateColorSelectorDlg(DxColor::ColorArgb color, bool enableAlpha, CWnd* pParent /* = nullptr*/)
{
	return std::make_shared<ColorSelectorDlgImpl>(color, enableAlpha, pParent);
}