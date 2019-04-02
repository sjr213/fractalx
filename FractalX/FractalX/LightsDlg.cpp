#include "stdafx.h"
#include "LightsDlg.h"

#include "ColorSelectorDlg.h"
#include "ColorUtils.h"
#include <Gdiplus.h>
#include "Resource.h"

using namespace DxColor;
using namespace Gdiplus;
using namespace ColorUtils;

class CLightsDlgImpl : public CLightsDlg 
{
private:
	Lights m_lights;
	const Gdiplus::Rect m_rectDiffuse1;
	const Gdiplus::Rect m_rectDiffuse2;
	const Gdiplus::Rect m_rectDiffuse3;
	const Gdiplus::Rect m_rectSpectacular1;
	const Gdiplus::Rect m_rectSpectacular2;
	const Gdiplus::Rect m_rectSpectacular3;

public:
	CLightsDlgImpl(const Lights& lights, CWnd* pParent)
		: CLightsDlg(IDD_LIGHTS_DLG, pParent)
		, m_lights(lights)
		, m_rectDiffuse1(50, 45, 40, 40)
		, m_rectDiffuse2(50, 99, 40, 40)
		, m_rectDiffuse3(50, 153, 40, 40)
		, m_rectSpectacular1(130, 45, 40, 40)
		, m_rectSpectacular2(130, 99, 40, 40)
		, m_rectSpectacular3(130, 153, 40, 40)
	{}

	DxColor::Lights GetLights() override
	{
		return m_lights;
	}

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override
	{
		CLightsDlg::OnInitDialog();
		EnableCtrls();

		return TRUE;
	}

	void EnableWnd(int id, bool enable)
	{
		CWnd* pWnd = GetDlgItem(id);
		if (pWnd)
			pWnd->EnableWindow(enable);
	}

	void EnableCtrls()
	{
		bool defaultLights = m_lights.DefaultLights;

		bool enableLight1 = m_lights.Light1.Enable && !defaultLights;
		EnableWnd(IDC_X1_EDIT, enableLight1);
		EnableWnd(IDC_Y1_EDIT, enableLight1);
		EnableWnd(IDC_Z1_EDIT, enableLight1);
		EnableWnd(IDC_ENABLE1_CHECK, !defaultLights);

		bool enableLight2 = m_lights.Light2.Enable && !defaultLights;
		EnableWnd(IDC_X2_EDIT, enableLight2);
		EnableWnd(IDC_Y2_EDIT, enableLight2);
		EnableWnd(IDC_Z2_EDIT, enableLight2);
		EnableWnd(IDC_ENABLE2_CHECK, !defaultLights);

		bool enableLight3 = m_lights.Light3.Enable && !defaultLights;
		EnableWnd(IDC_X3_EDIT, enableLight3);
		EnableWnd(IDC_Y3_EDIT, enableLight3);
		EnableWnd(IDC_Z3_EDIT, enableLight3);
		EnableWnd(IDC_ENABLE3_CHECK, !defaultLights);
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CLightsDlg::DoDataExchange(pDX);

		float l1x = std::get<0>(m_lights.Light1.Direction);
		DDX_Text(pDX, IDC_X1_EDIT, l1x);
		DDV_MinMaxFloat(pDX, l1x, -100.0f, 100.0f);
		std::get<0>(m_lights.Light1.Direction) = l1x;

		float l1y = std::get<2>(m_lights.Light1.Direction);
		DDX_Text(pDX, IDC_Y1_EDIT, l1y);
		DDV_MinMaxFloat(pDX, l1y, -100.0f, 100.0f);
		std::get<1>(m_lights.Light1.Direction) = l1y;

		float l1z = std::get<2>(m_lights.Light1.Direction);
		DDX_Text(pDX, IDC_Z1_EDIT, l1z);
		DDV_MinMaxFloat(pDX, l1z, -100.0f, 100.0f);
		std::get<2>(m_lights.Light1.Direction) = l1z;

		float l2x = std::get<0>(m_lights.Light2.Direction);
		DDX_Text(pDX, IDC_X2_EDIT, l2x);
		DDV_MinMaxFloat(pDX, l2x, -100.0f, 100.0f);
		std::get<0>(m_lights.Light2.Direction) = l2x;

		float l2y = std::get<2>(m_lights.Light2.Direction);
		DDX_Text(pDX, IDC_Y2_EDIT, l2x);
		DDV_MinMaxFloat(pDX, l2y, -100.0f, 100.0f);
		std::get<1>(m_lights.Light2.Direction) = l2y;

		float l2z = std::get<2>(m_lights.Light2.Direction);
		DDX_Text(pDX, IDC_Z2_EDIT, l2z);
		DDV_MinMaxFloat(pDX, l2z, -100.0f, 100.0f);
		std::get<2>(m_lights.Light2.Direction) = l2z;

		float l3x = std::get<0>(m_lights.Light3.Direction);
		DDX_Text(pDX, IDC_X3_EDIT, l3x);
		DDV_MinMaxFloat(pDX, l3x, -100.0f, 100.0f);
		std::get<0>(m_lights.Light3.Direction) = l3x;

		float l3y = std::get<2>(m_lights.Light3.Direction);
		DDX_Text(pDX, IDC_Y3_EDIT, l3y);
		DDV_MinMaxFloat(pDX, l3y, -100.0f, 100.0f);
		std::get<1>(m_lights.Light3.Direction) = l3y;

		float l3z = std::get<2>(m_lights.Light3.Direction);
		DDX_Text(pDX, IDC_Z3_EDIT, l3z);
		DDV_MinMaxFloat(pDX, l3z, -100.0f, 100.0f);
		std::get<2>(m_lights.Light3.Direction) = l3z;

		BOOL enableLight1 = m_lights.Light1.Enable;
		DDX_Check(pDX, IDC_ENABLE1_CHECK, enableLight1);
		m_lights.Light1.Enable = enableLight1;

		BOOL enableLight2 = m_lights.Light2.Enable;
		DDX_Check(pDX, IDC_ENABLE2_CHECK, enableLight2);
		m_lights.Light2.Enable = enableLight2;

		BOOL enableLight3 = m_lights.Light3.Enable;
		DDX_Check(pDX, IDC_ENABLE3_CHECK, enableLight3);
		m_lights.Light3.Enable = enableLight3;

		BOOL defaultLights = m_lights.DefaultLights;
		DDX_Check(pDX, IDC_DEFAULT_CHECK, defaultLights);
		m_lights.DefaultLights = defaultLights;

		BOOL perPixelLighting = m_lights.PerPixelLighting;
		DDX_Check(pDX, IDC_PER_PIXEL_CHECK, perPixelLighting);
		m_lights.PerPixelLighting = perPixelLighting;

		float alpha = m_lights.Alpha;
		DDX_Text(pDX, IDC_ALPHA_EDIT, alpha);
		DDV_MinMaxFloat(pDX, alpha, 0.0f, 1.0f);
		m_lights.Alpha = alpha;
	}

	// can add to common location
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

		const DxColor::ColorArgb blank = DxColor::ColorArgb(0, 0, 0, 0);

		bool draw1 = !m_lights.DefaultLights && m_lights.Light1.Enable;
		DrawColorSquare(graphics, m_rectDiffuse1, draw1 ? m_lights.Light1.Diffuse : blank);

		bool draw2 = !m_lights.DefaultLights && m_lights.Light2.Enable;
		DrawColorSquare(graphics, m_rectDiffuse2, draw2 ? m_lights.Light2.Diffuse : blank);

		bool draw3 = !m_lights.DefaultLights && m_lights.Light3.Enable;
		DrawColorSquare(graphics, m_rectDiffuse3, draw3 ? m_lights.Light3.Diffuse : blank);

		DrawColorSquare(graphics, m_rectSpectacular1, draw1 ? m_lights.Light1.Spectacular : blank);

		DrawColorSquare(graphics, m_rectSpectacular2, draw2 ? m_lights.Light2.Spectacular : blank);

		DrawColorSquare(graphics, m_rectSpectacular3, draw3 ? m_lights.Light3.Spectacular : blank);
	}

	// can add to common location
	bool PointInRect(const Gdiplus::Rect& rect, const CPoint& pt)
	{
		if (pt.x < rect.GetLeft() || pt.x > rect.GetRight())
			return false;

		if (pt.y < rect.GetTop() || pt.y > rect.GetBottom())
			return false;

		return true;
	}

	// can add to common location
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
		if (m_lights.DefaultLights)
			return;

		if (PointInRect(m_rectDiffuse1, point))
		{
			if (!m_lights.Light1.Enable)
				return;

			ChooseColor(m_lights.Light1.Diffuse);
			return;
		}

		if (PointInRect(m_rectDiffuse2, point))
		{
			if (!m_lights.Light2.Enable)
				return;

			ChooseColor(m_lights.Light2.Diffuse);
			return;
		}

		if (PointInRect(m_rectDiffuse3, point))
		{
			if (!m_lights.Light3.Enable)
				return;

			ChooseColor(m_lights.Light3.Diffuse);
			return;
		}

		if (PointInRect(m_rectSpectacular1, point))
		{
			if (!m_lights.Light1.Enable)
				return;

			ChooseColor(m_lights.Light1.Spectacular);
			return;
		}

		if (PointInRect(m_rectSpectacular2, point))
		{
			if (!m_lights.Light2.Enable)
				return;

			ChooseColor(m_lights.Light2.Spectacular);
			return;
		}

		if (PointInRect(m_rectSpectacular3, point))
		{
			if (!m_lights.Light3.Enable)
				return;

			ChooseColor(m_lights.Light3.Spectacular);
			return;
		}
	}

	void OnKillFocusDirection()
	{
		UpdateData(TRUE);
	}

	void OnCheckBoxClicked()
	{
		UpdateData(TRUE);
		EnableCtrls();
		Invalidate(TRUE);
	}

};

BEGIN_MESSAGE_MAP(CLightsDlgImpl, CLightsDlg)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_EN_KILLFOCUS(IDC_X1_EDIT, &CLightsDlgImpl::OnKillFocusDirection)
	ON_EN_KILLFOCUS(IDC_Y1_EDIT, &CLightsDlgImpl::OnKillFocusDirection)
	ON_EN_KILLFOCUS(IDC_Z1_EDIT, &CLightsDlgImpl::OnKillFocusDirection)
	ON_EN_KILLFOCUS(IDC_X2_EDIT, &CLightsDlgImpl::OnKillFocusDirection)
	ON_EN_KILLFOCUS(IDC_Y2_EDIT, &CLightsDlgImpl::OnKillFocusDirection)
	ON_EN_KILLFOCUS(IDC_Z2_EDIT, &CLightsDlgImpl::OnKillFocusDirection)
	ON_EN_KILLFOCUS(IDC_X3_EDIT, &CLightsDlgImpl::OnKillFocusDirection)
	ON_EN_KILLFOCUS(IDC_Y3_EDIT, &CLightsDlgImpl::OnKillFocusDirection)
	ON_EN_KILLFOCUS(IDC_Z3_EDIT, &CLightsDlgImpl::OnKillFocusDirection)
	ON_EN_KILLFOCUS(IDC_ALPHA_EDIT, &CLightsDlgImpl::OnKillFocusDirection)
	ON_BN_CLICKED(IDC_DEFAULT_CHECK, &CLightsDlgImpl::OnCheckBoxClicked)
	ON_BN_CLICKED(IDC_ENABLE1_CHECK, &CLightsDlgImpl::OnCheckBoxClicked)
	ON_BN_CLICKED(IDC_ENABLE2_CHECK, &CLightsDlgImpl::OnCheckBoxClicked)
	ON_BN_CLICKED(IDC_ENABLE3_CHECK, &CLightsDlgImpl::OnCheckBoxClicked)
END_MESSAGE_MAP()

std::shared_ptr<CLightsDlg> CLightsDlg::CreateLightsDlg(const DxColor::Lights& lights, CWnd* pParent)
{
	return std::make_shared<CLightsDlgImpl>(lights, pParent);
}