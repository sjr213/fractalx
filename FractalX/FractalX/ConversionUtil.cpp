#include "stdafx.h"
#include "ConversionUtil.h"

#include "ColorUtils.h"

#include "EffectColors.h"
#include "Light.h"

namespace
{
	DXF::DxLight ConvertLight(const DxColor::Light& light)
	{
		DXF::DxLight dxLight;
		dxLight.Enable = light.Enable;
		dxLight.DiffuseColor = ColorUtils::ConvertToDxColor(light.Diffuse);
		dxLight.SpectacularColor = ColorUtils::ConvertToDxColor(light.Spectacular);
		dxLight.Direction = DirectX::SimpleMath::Vector3(std::get<0>(light.Direction), std::get<1>(light.Direction), std::get<2>(light.Direction));

		return dxLight;
	}
}

namespace ConversionUtil
{

	DXF::DxEffectColors ConvertEffectColors(const DxColor::EffectColors& effectColors)
	{
		DXF::DxEffectColors dxColors;
		dxColors.AmbientColor = ColorUtils::ConvertToDxColor(effectColors.Ambient);
		dxColors.DiffuseColor = ColorUtils::ConvertToDxColor(effectColors.Diffuse);
		dxColors.SpecularColor = ColorUtils::ConvertToDxColor(effectColors.Specular);
		dxColors.EmissiveColor = ColorUtils::ConvertToDxColor(effectColors.Emissive);
		dxColors.SpecularPower = effectColors.SpecularPower;

		return dxColors;
	}

	DXF::DxLights ConvertLights(const DxColor::Lights& lights)
	{
		DXF::DxLights dxLights;
		dxLights.DefaultLights = lights.DefaultLights;
		dxLights.PerPixelLighting = lights.PerPixelLighting;
		dxLights.Alpha = lights.Alpha;

		dxLights.Lights.at(0) = ConvertLight(lights.Light1);
		dxLights.Lights.at(1) = ConvertLight(lights.Light2);
		dxLights.Lights.at(2) = ConvertLight(lights.Light3);

		return dxLights;
	}

	CRect MakeRect(CPoint pt1, CPoint pt2)
	{
		int lf = std::min<int>(pt1.x, pt2.x);
		int rt = std::max<int>(pt1.x, pt2.x);
		int tp = std::min<int>(pt1.y, pt2.y);
		int bt = std::max<int>(pt1.y, pt2.y);

		return CRect(lf, tp, rt, bt);
	}
}