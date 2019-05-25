#include "stdafx.h"
#include "SinglePinEditDlg.h"

#include "ColorContrast.h"
#include "ColorUtils.h"
#include <Gdiplus.h>
#include "hslUtilities.h"
#include "Resource.h"

using namespace DxColor;
using namespace ColorUtils;

class CSinglePinEditDlgImpl : public CSinglePinEditDlg
{
private:
	std::vector<DxColor::ColorPin> m_pins;
	int m_pinIndex;
	DxColor::ColorArgb m_rawColor;
	COLORREF m_rgb;
	HSL m_hsl;
	bool m_enableAlpha;
	CComboBox m_colorCombo;
	int m_colorIndex = 0;

public:
	CSinglePinEditDlgImpl(const std::vector<DxColor::ColorPin>& pins, int pinIndex, bool enableAlpha, CWnd* pParent)
		: CSinglePinEditDlg(IDD_EDIT_SINGLE_PIN_DLG, pParent)
		, m_pins(pins)
		, m_pinIndex(pinIndex)
		, m_enableAlpha(enableAlpha)
	{
		SetColors();
	}

	virtual ~CSinglePinEditDlgImpl() {}

	std::vector<DxColor::ColorPin> GetPins() override
	{
		return m_pins;
	}

protected:
	DECLARE_MESSAGE_MAP()

	void SetColors()
	{
		m_rawColor = (m_colorIndex == 1) ? m_pins.at(m_pinIndex).Color2 : m_pins.at(m_pinIndex).Color1;
		m_rgb = ToColorRef(m_rawColor);
		m_hsl = ToHsl(m_rawColor);
	}

	BOOL OnInitDialog() override
	{
		CSinglePinEditDlg::OnInitDialog();

		PopulateColorCombo();

		EnableCtrls();

		return TRUE;
	}

	void PopulateColorCombo()
	{
		m_colorCombo.ResetContent();

		m_colorCombo.AddString(_T("Color 1"));
		m_colorCombo.AddString(_T("Color 2"));

		m_colorCombo.SetCurSel(m_colorIndex);
	}

	void EnableCtrls()
	{
		CWnd* pWnd = GetDlgItem(IDC_ALPHA_EDIT);
		if (pWnd)
			pWnd->EnableWindow(m_enableAlpha);
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CSinglePinEditDlg::DoDataExchange(pDX);

		DDX_Control(pDX, IDC_COLOR_COMBO, m_colorCombo);

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

		SetPinColor();

		UpdateData(FALSE);
		Invalidate(TRUE);
	}

	void OnKillFocusHsl()
	{
		UpdateData(TRUE);

		m_rawColor = ToRgb(m_hsl, m_rawColor.A);
		m_rgb = ToColorRef(m_rawColor);

		SetPinColor();

		UpdateData(FALSE);
		Invalidate(TRUE);
	}

	void SetPinColor()
	{
		if (m_colorIndex == 0)
			m_pins.at(m_pinIndex).Color1 = m_rawColor;
		else if (m_colorIndex == 1)
			m_pins.at(m_pinIndex).Color2 = m_rawColor;
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

			SetPinColor();

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

		HatchBrush backGroundBrush(Gdiplus::HatchStyle::HatchStyleWideDownwardDiagonal, Color::Black, Color::White);

		graphics.FillRectangle(&backGroundBrush, rect);

		SolidBrush colorBrush(ConvertToGdiColor(m_rawColor));

		graphics.FillRectangle(&colorBrush, rect);

		// Create a Pen object.
		Pen blackPen(Color(255, 0, 0, 0), 2);

		// Draw the rectangle.
		graphics.DrawRectangle(&blackPen, rect);
	}

	void OnCbnSelchangeColorCombo()
	{
		UpdateData(TRUE);

		SetPinColor();
		
		int index = m_colorCombo.GetCurSel();
		if (index == CB_ERR)
		{
			m_colorIndex = 0;
			m_colorCombo.SetCurSel(m_colorIndex);	
		}
		else
		{
			m_colorIndex = index;
		}

		SetColors();

		UpdateData(FALSE);
		Invalidate(TRUE);
	}
};

BEGIN_MESSAGE_MAP(CSinglePinEditDlgImpl, CSinglePinEditDlg)
	ON_WM_PAINT()
	ON_EN_KILLFOCUS(IDC_RED_EDIT, &CSinglePinEditDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_GREEN_EDIT, &CSinglePinEditDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_BLUE_EDIT, &CSinglePinEditDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_ALPHA_EDIT, &CSinglePinEditDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_HUE_EDIT, &CSinglePinEditDlgImpl::OnKillFocusHsl)
	ON_EN_KILLFOCUS(IDC_SATURATION_EDIT, &CSinglePinEditDlgImpl::OnKillFocusHsl)
	ON_EN_KILLFOCUS(IDC_LIGHTNESS_EDIT, &CSinglePinEditDlgImpl::OnKillFocusHsl)
	ON_BN_CLICKED(IDC_COLOR_CHOOSER_BUT, &CSinglePinEditDlgImpl::OnBnColorChooser)
	ON_CBN_SELCHANGE(IDC_COLOR_COMBO, &CSinglePinEditDlgImpl::OnCbnSelchangeColorCombo)
END_MESSAGE_MAP()

std::shared_ptr<CSinglePinEditDlg> CSinglePinEditDlg::CreateSinglePinEditDlg(const std::vector<ColorPin>& pins, int pinIndex, bool enableAlpha, CWnd* pParent /* = nullptr*/)
{
	return std::make_shared<CSinglePinEditDlgImpl>(pins, pinIndex, enableAlpha, pParent);
}