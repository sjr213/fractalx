#include "stdafx.h"
#include "EffectColorDlg.h"

#include "ColorSelectorDlg.h"
#include "ColorUtils.h"
#include "DialogUtils.h"
#include <Gdiplus.h>
#include "NarrowCast.h"
#include "Resource.h"

using namespace DlgUtils;
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
	const Gdiplus::Rect m_rectEmissive;
	DXF::DxVersion m_modelVersion = DXF::DxVersion::Dx11;

public:
	EffectColorDlgImpl(EffectColors colors, DXF::DxVersion version, CWnd* pParent)
		: CEffectColorDlg(IDD_EFFECT_COLOR_DLG, pParent)
		, m_colors(colors)
		, m_rectAmbient(100, 45, 40, 40)
		, m_rectDiffuse(100, 104, 40, 40)
		, m_rectSpecular(100, 163, 40, 40)
		, m_rectEmissive(100, 222, 40, 40)
		, m_modelVersion(version)
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

		EnableCtrls();

		return TRUE;
	}

	void EnableCtrls()
	{
		auto specPowerCtrl = GetDlgItem(IDC_SPECULAR_POWER_EDIT);
		if (specPowerCtrl)
			specPowerCtrl->EnableWindow(m_modelVersion == DXF::DxVersion::Dx11);

		auto roughnessCtrl = GetDlgItem(IDC_ROUGHNESS_EDIT);
		if (roughnessCtrl)
			roughnessCtrl->EnableWindow(m_modelVersion == DXF::DxVersion::Dx12);
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CEffectColorDlg::DoDataExchange(pDX);

		int ambientRed = m_colors.Ambient.R;
		DDX_Text(pDX, IDC_AMBIENT_RED_EDIT, ambientRed);
		DDV_MinMaxInt(pDX, ambientRed, 0, 255);
		m_colors.Ambient.R = DxCore::narrow_cast<bite>(ambientRed);

		int ambientGreen = m_colors.Ambient.G;
		DDX_Text(pDX, IDC_AMBIENT_GREEN_EDIT, ambientGreen);
		DDV_MinMaxInt(pDX, ambientGreen, 0, 255);
		m_colors.Ambient.G = DxCore::narrow_cast<bite>(ambientGreen);

		int ambientBlue = m_colors.Ambient.B;
		DDX_Text(pDX, IDC_AMBIENT_BLUE_EDIT, ambientBlue);
		DDV_MinMaxInt(pDX, ambientBlue, 0, 255);
		m_colors.Ambient.B = DxCore::narrow_cast<bite>(ambientBlue);

		int diffuseRed = m_colors.Diffuse.R;
		DDX_Text(pDX, IDC_DIFFUSE_RED_EDIT, diffuseRed);
		DDV_MinMaxInt(pDX, diffuseRed, 0, 255);
		m_colors.Diffuse.R = DxCore::narrow_cast<bite>(diffuseRed);

		int diffuseGreen = m_colors.Diffuse.G;
		DDX_Text(pDX, IDC_DIFFUSE_GREEN_EDIT, diffuseGreen);
		DDV_MinMaxInt(pDX, diffuseGreen, 0, 255);
		m_colors.Diffuse.G = DxCore::narrow_cast<bite>(diffuseGreen);

		int diffuseBlue = m_colors.Diffuse.B;
		DDX_Text(pDX, IDC_DIFFUSE_BLUE_EDIT, diffuseBlue);
		DDV_MinMaxInt(pDX, diffuseBlue, 0, 255);
		m_colors.Diffuse.B = DxCore::narrow_cast<bite>(diffuseBlue);

		int specularRed = m_colors.Specular.R;
		DDX_Text(pDX, IDC_SPECULAR_RED_EDIT, specularRed);
		DDV_MinMaxInt(pDX, specularRed, 0, 255);
		m_colors.Specular.R = DxCore::narrow_cast<bite>(specularRed);

		int specularGreen = m_colors.Specular.G;
		DDX_Text(pDX, IDC_SPECULAR_GREEN_EDIT, specularGreen);
		DDV_MinMaxInt(pDX, specularGreen, 0, 255);
		m_colors.Specular.G = DxCore::narrow_cast<bite>(specularGreen);

		int specularBlue = m_colors.Specular.B;
		DDX_Text(pDX, IDC_SPECULAR_BLUE_EDIT, specularBlue);
		DDV_MinMaxInt(pDX, specularBlue, 0, 255);
		m_colors.Specular.B = DxCore::narrow_cast<bite>(specularBlue);

		int emissiveRed = m_colors.Emissive.R;
		DDX_Text(pDX, IDC_EMISSIVE_RED_EDIT, emissiveRed);
		DDV_MinMaxInt(pDX, emissiveRed, 0, 255);
		m_colors.Emissive.R = DxCore::narrow_cast<bite>(emissiveRed);

		int emissiveGreen = m_colors.Emissive.G;
		DDX_Text(pDX, IDC_EMISSIVE_GREEN_EDIT, emissiveGreen);
		DDV_MinMaxInt(pDX, emissiveGreen, 0, 255);
		m_colors.Emissive.G = DxCore::narrow_cast<bite>(emissiveGreen);

		int emissiveBlue = m_colors.Emissive.B;
		DDX_Text(pDX, IDC_EMISSIVE_BLUE_EDIT, emissiveBlue);
		DDV_MinMaxInt(pDX, emissiveBlue, 0, 255);
		m_colors.Emissive.B = DxCore::narrow_cast<bite>(emissiveBlue);

		float specularPower = m_colors.SpecularPower;
		DDX_Text(pDX, IDC_SPECULAR_POWER_EDIT, specularPower);
		DDV_MinMaxFloat(pDX, specularPower, 0.0f, 1000.0f);
		m_colors.SpecularPower = specularPower;

		float roughness = m_colors.Roughness;
		DDX_Text(pDX, IDC_ROUGHNESS_EDIT, roughness);
		DDV_MinMaxFloat(pDX, roughness, 0.0f, 1.0f);
		m_colors.Roughness = roughness;
	}

	void OnKillFocusRGB()
	{
		UpdateData(TRUE);
		Invalidate(TRUE);
	}

	void OnPaint()
	{
		using namespace Gdiplus;

		CPaintDC dc(this);
		Graphics graphics(dc);

		DrawColorSquare(graphics, m_rectAmbient, m_colors.Ambient);

		DrawColorSquare(graphics, m_rectDiffuse, m_colors.Diffuse);

		DrawColorSquare(graphics, m_rectSpecular, m_colors.Specular);

		DrawColorSquare(graphics, m_rectEmissive, m_colors.Emissive);
	}

	void OnDefault()
	{
		EffectColors::SetDefaultEffectColors(m_colors);
		UpdateData(FALSE);
		Invalidate(TRUE);
	}

	void OnLButtonUp(UINT /*nFlags*/, CPoint point)
	{
		if (PointInRect(m_rectAmbient, point))
		{
			ChooseColor(m_colors.Ambient, false, *this);
			return;
		}

		if (PointInRect(m_rectDiffuse, point))
		{
			ChooseColor(m_colors.Diffuse, false, *this);
			return;
		}

		if (PointInRect(m_rectSpecular, point))
		{
			ChooseColor(m_colors.Specular, false, *this);
			return;
		}

		if (PointInRect(m_rectEmissive, point))
		{
			ChooseColor(m_colors.Emissive, false, *this);
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
	ON_EN_KILLFOCUS(IDC_DIFFUSE_RED_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_DIFFUSE_GREEN_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_DIFFUSE_BLUE_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_SPECULAR_RED_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_SPECULAR_GREEN_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_SPECULAR_BLUE_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_EMISSIVE_RED_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_EMISSIVE_GREEN_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_EMISSIVE_BLUE_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_SPECULAR_POWER_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_EN_KILLFOCUS(IDC_ROUGHNESS_EDIT, &EffectColorDlgImpl::OnKillFocusRGB)
	ON_BN_CLICKED(IDC_DEFAULT_BUT, &EffectColorDlgImpl::OnDefault)
END_MESSAGE_MAP()

std::shared_ptr<CEffectColorDlg> CEffectColorDlg::CreateEffectColorDlg(DxColor::EffectColors colors, DXF::DxVersion version, CWnd* pParent /*= nullptr*/)
{
	return std::make_shared<EffectColorDlgImpl>(colors, version, pParent);
}