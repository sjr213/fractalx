#include "stdafx.h"
#include "LightsDlg.h"

#include "ColorSelectorDlg.h"
#include "ColorUtils.h"
#include "DialogUtils.h"
#include <Gdiplus.h>
#include "Resource.h"
#include "UiUtilities.h"

using namespace DlgUtils;
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

	void EnableCtrls()
	{
		bool defaultLights = m_lights.DefaultLights;

		bool enableLight1 = m_lights.Light1.Enable && !defaultLights;
		UiUtilities::SafeEnable(this, IDC_X1_EDIT, enableLight1);
		UiUtilities::SafeEnable(this, IDC_Y1_EDIT, enableLight1);
		UiUtilities::SafeEnable(this, IDC_Z1_EDIT, enableLight1);
		UiUtilities::SafeEnable(this, IDC_ENABLE1_CHECK, !defaultLights);

		bool enableLight2 = m_lights.Light2.Enable && !defaultLights;
		UiUtilities::SafeEnable(this, IDC_X2_EDIT, enableLight2);
		UiUtilities::SafeEnable(this, IDC_Y2_EDIT, enableLight2);
		UiUtilities::SafeEnable(this, IDC_Z2_EDIT, enableLight2);
		UiUtilities::SafeEnable(this, IDC_ENABLE2_CHECK, !defaultLights);

		bool enableLight3 = m_lights.Light3.Enable && !defaultLights;
		UiUtilities::SafeEnable(this, IDC_X3_EDIT, enableLight3);
		UiUtilities::SafeEnable(this, IDC_Y3_EDIT, enableLight3);
		UiUtilities::SafeEnable(this, IDC_Z3_EDIT, enableLight3);
		UiUtilities::SafeEnable(this, IDC_ENABLE3_CHECK, !defaultLights);
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CLightsDlg::DoDataExchange(pDX);

		float l1x = -1.0f * std::get<0>(m_lights.Light1.Direction);
		DDX_Text(pDX, IDC_X1_EDIT, l1x);
		DDV_MinMaxFloat(pDX, l1x, -100.0f, 100.0f);
		std::get<0>(m_lights.Light1.Direction) = -1.0f *l1x;

		float l1y = -1.0f * std::get<1>(m_lights.Light1.Direction);
		DDX_Text(pDX, IDC_Y1_EDIT, l1y);
		DDV_MinMaxFloat(pDX, l1y, -100.0f, 100.0f);
		std::get<1>(m_lights.Light1.Direction) = -1.0f * l1y;

		float l1z = -1.0f * std::get<2>(m_lights.Light1.Direction);
		DDX_Text(pDX, IDC_Z1_EDIT, l1z);
		DDV_MinMaxFloat(pDX, l1z, -100.0f, 100.0f);
		std::get<2>(m_lights.Light1.Direction) = -1.0f * l1z;

		float l2x = -1.0f * std::get<0>(m_lights.Light2.Direction);
		DDX_Text(pDX, IDC_X2_EDIT, l2x);
		DDV_MinMaxFloat(pDX, l2x, -100.0f, 100.0f);
		std::get<0>(m_lights.Light2.Direction) = -1.0f * l2x;

		float l2y = -1.0f * std::get<1>(m_lights.Light2.Direction);
		DDX_Text(pDX, IDC_Y2_EDIT, l2y);
		DDV_MinMaxFloat(pDX, l2y, -100.0f, 100.0f);
		std::get<1>(m_lights.Light2.Direction) = -1.0f * l2y;

		float l2z = -1.0f * std::get<2>(m_lights.Light2.Direction);
		DDX_Text(pDX, IDC_Z2_EDIT, l2z);
		DDV_MinMaxFloat(pDX, l2z, -100.0f, 100.0f);
		std::get<2>(m_lights.Light2.Direction) = -1.0f * l2z;

		float l3x = -1.0f * std::get<0>(m_lights.Light3.Direction);
		DDX_Text(pDX, IDC_X3_EDIT, l3x);
		DDV_MinMaxFloat(pDX, l3x, -100.0f, 100.0f);
		std::get<0>(m_lights.Light3.Direction) = -1.0f * l3x;

		float l3y = -1.0f * std::get<1>(m_lights.Light3.Direction);
		DDX_Text(pDX, IDC_Y3_EDIT, l3y);
		DDV_MinMaxFloat(pDX, l3y, -100.0f, 100.0f);
		std::get<1>(m_lights.Light3.Direction) = -1.0f * l3y;

		float l3z = -1.0f * std::get<2>(m_lights.Light3.Direction);
		DDX_Text(pDX, IDC_Z3_EDIT, l3z);
		DDV_MinMaxFloat(pDX, l3z, -100.0f, 100.0f);
		std::get<2>(m_lights.Light3.Direction) = -1.0f * l3z;

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

	void OnLButtonUp(UINT /*nFlags*/, CPoint point)
	{
		if (m_lights.DefaultLights)
			return;

		if (PointInRect(m_rectDiffuse1, point))
		{
			if (!m_lights.Light1.Enable)
				return;

			ChooseColor(m_lights.Light1.Diffuse, false, *this);
			return;
		}

		if (PointInRect(m_rectDiffuse2, point))
		{
			if (!m_lights.Light2.Enable)
				return;

			ChooseColor(m_lights.Light2.Diffuse, false, *this);
			return;
		}

		if (PointInRect(m_rectDiffuse3, point))
		{
			if (!m_lights.Light3.Enable)
				return;

			ChooseColor(m_lights.Light3.Diffuse, false, *this);
			return;
		}

		if (PointInRect(m_rectSpectacular1, point))
		{
			if (!m_lights.Light1.Enable)
				return;

			ChooseColor(m_lights.Light1.Spectacular, false, *this);
			return;
		}

		if (PointInRect(m_rectSpectacular2, point))
		{
			if (!m_lights.Light2.Enable)
				return;

			ChooseColor(m_lights.Light2.Spectacular, false, *this);
			return;
		}

		if (PointInRect(m_rectSpectacular3, point))
		{
			if (!m_lights.Light3.Enable)
				return;

			ChooseColor(m_lights.Light3.Spectacular, false, *this);
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