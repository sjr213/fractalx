#include "stdafx.h"
#include "EffectColorDlg.h"

#include "ColorSelectorDlg.h"
#include "ColorUtils.h"
#include <Gdiplus.h>
#include "Resource.h"

using namespace DxColor;
using namespace Gdiplus;
using namespace ColorUtils;

class EffectColorDlgImpl : public CEffectColorDlg
{
private:
	DxColor::EffectColors m_colors;
	const Gdiplus::Rect m_rectAmbient;
	const Gdiplus::Rect m_rectDiffuse;
	const Gdiplus::Rect m_rectSpecular;

public:
	EffectColorDlgImpl(EffectColors colors, CWnd* pParent)
		: CEffectColorDlg(IDD_EFFECT_COLOR_DLG, pParent)
		, m_colors(colors)
		, m_rectAmbient(100, 45, 40, 40)
		, m_rectDiffuse(100, 105, 40, 40)
		, m_rectSpecular(100, 165, 40, 40)
	{}

	virtual ~EffectColorDlgImpl() {}

	DxColor::EffectColors GetColors() override
	{
		return m_colors;
	}

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override
	{
		CEffectColorDlg::OnInitDialog();

		return TRUE;
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CEffectColorDlg::DoDataExchange(pDX);

		int ambientRed = m_colors.Ambient.R;
		DDX_Text(pDX, IDC_AMBIENT_RED_EDIT, ambientRed);
		DDV_MinMaxInt(pDX, ambientRed, 0, 255);
		m_colors.Ambient.R = ambientRed;

		int ambientGreen = m_colors.Ambient.G;
		DDX_Text(pDX, IDC_AMBIENT_GREEN_EDIT, ambientGreen);
		DDV_MinMaxInt(pDX, ambientGreen, 0, 255);
		m_colors.Ambient.G = ambientGreen;

		int ambientBlue = m_colors.Ambient.B;
		DDX_Text(pDX, IDC_AMBIENT_BLUE_EDIT, ambientBlue);
		DDV_MinMaxInt(pDX, ambientBlue, 0, 255);
		m_colors.Ambient.B = ambientBlue;

		int ambientAlpha = m_colors.Ambient.A;
		DDX_Text(pDX, IDC_AMBIENT_ALPHA_EDIT, ambientAlpha);
		DDV_MinMaxInt(pDX, ambientAlpha, 0, 255);
		m_colors.Ambient.A = ambientAlpha;

		int diffuseRed = m_colors.Diffuse.R;
		DDX_Text(pDX, IDC_DIFFUSE_RED_EDIT, diffuseRed);
		DDV_MinMaxInt(pDX, diffuseRed, 0, 255);
		m_colors.Diffuse.R = diffuseRed;

		int diffuseGreen = m_colors.Diffuse.G;
		DDX_Text(pDX, IDC_DIFFUSE_GREEN_EDIT, diffuseGreen);
		DDV_MinMaxInt(pDX, diffuseGreen, 0, 255);
		m_colors.Diffuse.G = diffuseGreen;

		int diffuseBlue = m_colors.Diffuse.B;
		DDX_Text(pDX, IDC_DIFFUSE_BLUE_EDIT, diffuseBlue);
		DDV_MinMaxInt(pDX, diffuseBlue, 0, 255);
		m_colors.Diffuse.B = diffuseBlue;

		int diffuseAlpha = m_colors.Diffuse.A;
		DDX_Text(pDX, IDC_DIFFUSE_ALPHA_EDIT, diffuseAlpha);
		DDV_MinMaxInt(pDX, diffuseAlpha, 0, 255);
		m_colors.Diffuse.A = diffuseAlpha;

		int specularRed = m_colors.Specular.R;
		DDX_Text(pDX, IDC_SPECULAR_RED_EDIT, specularRed);
		DDV_MinMaxInt(pDX, specularRed, 0, 255);
		m_colors.Specular.R = specularRed;

		int specularGreen = m_colors.Specular.G;
		DDX_Text(pDX, IDC_SPECULAR_GREEN_EDIT, specularGreen);
		DDV_MinMaxInt(pDX, specularGreen, 0, 255);
		m_colors.Specular.G = specularGreen;

		int specularBlue = m_colors.Specular.B;
		DDX_Text(pDX, IDC_SPECULAR_BLUE_EDIT, specularBlue);
		DDV_MinMaxInt(pDX, specularBlue, 0, 255);
		m_colors.Specular.B = specularBlue;

		int specularAlpha = m_colors.Specular.A;
		DDX_Text(pDX, IDC_SPECULAR_ALPHA_EDIT, specularAlpha);
		DDV_MinMaxInt(pDX, specularAlpha, 0, 255);
		m_colors.Specular.A = specularAlpha;

		float specularPower = m_colors.SpecularPower;
		DDX_Text(pDX, IDC_SPECULAR_POWER_EDIT, specularPower);
		DDV_MinMaxFloat(pDX, specularPower, 0.0f, 1000.0f);
		m_colors.SpecularPower = specularPower;
	}

	void OnKillFocusRGB()
	{
		UpdateData(TRUE);
		Invalidate(TRUE);
	}

	void DrawColorSquare(Gdiplus::Graphics& graphics, const Gdiplus::Rect& rect, DxColor::ColorArgb color)
	{
		HatchBrush backGroundBrush(HatchStyle::HatchStyleWideDownwardDiagonal, Color::Black, Color::White);

		graphics.FillRectangle(&backGroundBrush, rect);

		SolidBrush colorBrush(ConvertToGdiColor(color));

		graphics.FillRectangle(&colorBrush, rect);

		// Create a Pen object.
		Pen blackPen(Color(255, 0, 0, 0), 2);

		// Draw the rectangle.
		graphics.DrawRectangle(&blackPen, rect);
	}

	void OnPaint()
	{
		using namespace Gdiplus;

		CPaintDC dc(this);
		Graphics graphics(dc);

		DrawColorSquare(graphics, m_rectAmbient, m_colors.Ambient);

		DrawColorSquare(graphics, m_rectDiffuse, m_colors.Diffuse);

		DrawColorSquare(graphics, m_rectSpecular, m_colors.Specular);
	}

	void OnDefault()
	{
		EffectColors::SetDefaultEffectColors(m_colors);
		UpdateData(FALSE);
		Invalidate(TRUE);
	}

	bool PointInRect(const Gdiplus::Rect& rect, const CPoint& pt)
	{
		if (pt.x < rect.GetLeft() || pt.x > rect.GetRight())
			return false;

		if (pt.y < rect.GetTop() || pt.y > rect.GetBottom())
			return false;

		return true;
	}

	void ChooseColor(DxColor::ColorArgb& color)
	{
		auto colorDlg = CColorSelectorDlg::CreateColorSelectorDlg(color, this);
		if (colorDlg->DoModal() == IDOK)
		{
			color = colorDlg->GetColor();

			UpdateData(FALSE);
			Invalidate(TRUE);
		}
	}

	void OnLButtonUp(UINT /*nFlags*/, CPoint point)
	{
		if (PointInRect(m_rectAmbient, point))
		{
			ChooseColor(m_colors.Ambient);
			return;
		}

		if (PointInRect(m_rectDiffuse, point))
		{
			ChooseColor(m_colors.Diffuse);
			return;
		}

		if (PointInRect(m_rectSpecular, point))
		{
			ChooseColor(m_colors.Specular);
			return;
		}
	}

};

BEGIN_MESSAGE_MAP(EffectColorDlgImpl, CEffectColorDlg)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_EN_KILLFOCUS(IDC_AMBIENT_RED_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_AMBIENT_GREEN_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_AMBIENT_BLUE_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_AMBIENT_ALPHA_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_DIFFUSE_RED_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_DIFFUSE_GREEN_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_DIFFUSE_BLUE_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_DIFFUSE_ALPHA_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_SPECULAR_RED_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_SPECULAR_GREEN_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_SPECULAR_BLUE_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_SPECULAR_ALPHA_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_SPECULAR_POWER_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_BN_CLICKED(IDC_DEFAULT_BUT, &EffectColorDlgImpl::OnDefault)
END_MESSAGE_MAP()

std::shared_ptr<CEffectColorDlg> CEffectColorDlg::CreateEffectColorDlg(DxColor::EffectColors colors, CWnd* pParent /*= nullptr*/)
{
	return std::make_shared<EffectColorDlgImpl>(colors, pParent);
}