#include "stdafx.h"
#include "DxColorsSerialization.h"

#include <afx.h>
#include <assert.h>
#include "ColorArgb.h"
#include "ColorContrast.h"
#include "EffectColors.h"
#include "Light.h"


namespace DxColor
{
	void SerializeTuple3(CArchive& ar, std::tuple<float, float, float>& f3)
	{
		if (ar.IsStoring())
		{
			ar << std::get<0>(f3) << std::get<1>(f3) << std::get<2>(f3);
		}
		else
		{
			ar >> std::get<0>(f3) >> std::get<1>(f3) >> std::get<2>(f3);
		}
	}

	void SerializeColorArgb(CArchive& ar, ColorArgb& color)
	{
		if (ar.IsStoring())
			ar << color.A << color.R << color.G << color.B;
		else
		{
			// ar >> color.A >> color.R << color.G << color.B;
			BYTE a, r, g, b;
			ar >> a >> r >> g >> b;

			color.A = a;
			color.R = r;
			color.G = g;
			color.B = b;
		}
	}

	void SerializeLight(CArchive& ar, Light& light)
	{
		const int LightVersion = 1;

		if (ar.IsStoring())
		{
			ar << LightVersion;
			ar << light.Enable;
			SerializeColorArgb(ar, light.Diffuse);
			SerializeColorArgb(ar, light.Spectacular);
			SerializeTuple3(ar, light.Direction);
		}
		else
		{
			int version = 0;
			ar >> version;

			if (version < LightVersion)
			{
				assert(false);
				return;
			}

			ar >> light.Enable;
			SerializeColorArgb(ar, light.Diffuse);
			SerializeColorArgb(ar, light.Spectacular);
			SerializeTuple3(ar, light.Direction);
		}
	}

	void Serialize(CArchive& ar, ColorContrast& contrast)
	{
		const int ContrastVersion = 1;

		if (ar.IsStoring())
		{
			ar << ContrastVersion;
			ar << static_cast<int>(contrast.Mode);
			ar << contrast.MinContrast[0] << contrast.MinContrast[1] << contrast.MinContrast[2];
			ar << contrast.MaxContrast[0] << contrast.MaxContrast[1] << contrast.MaxContrast[2];

			ar << contrast.MinHue << contrast.MaxHue;
			ar << contrast.MinSaturation << contrast.MaxSaturation;
			ar << contrast.MinLightness << contrast.MaxLightness;
		}
		else
		{
			int version = 0;
			ar >> version;

			assert(version == ContrastVersion);
			if (version != ContrastVersion)
				return;

			int mode = 1;
			ar >> mode;
			ar >> contrast.MinContrast[0] >> contrast.MinContrast[1] >> contrast.MinContrast[2];
			ar >> contrast.MaxContrast[0] >> contrast.MaxContrast[1] >> contrast.MaxContrast[2];

			ar >> contrast.MinHue >> contrast.MaxHue;
			ar >> contrast.MinSaturation >> contrast.MaxSaturation;
			ar >> contrast.MinLightness >> contrast.MaxLightness;

			contrast.Mode = static_cast<ContrastType>(mode);
		}
	}

	void SerializeEffectColors(CArchive& ar, EffectColors& colors)
	{
		SerializeColorArgb(ar, colors.Ambient);
		SerializeColorArgb(ar, colors.Diffuse);
		SerializeColorArgb(ar, colors.Specular);
		SerializeColorArgb(ar, colors.Emissive);

		if (ar.IsStoring())
			ar << colors.SpecularPower;
		else
			ar >> colors.SpecularPower;
	}

	void SerializeLights(CArchive& ar, Lights& lights)
	{
		const int LightsVersion = 1;

		if (ar.IsStoring())
		{
			ar << LightsVersion;
			ar << lights.DefaultLights;
			ar << lights.PerPixelLighting;
			ar << lights.Alpha;
			SerializeLight(ar, lights.Light1);
			SerializeLight(ar, lights.Light2);
			SerializeLight(ar, lights.Light3);
		}
		else
		{
			int version = 0;
			ar >> version;

			if (version < LightsVersion)
			{
				assert(false);
				return;
			}

			ar >> lights.DefaultLights;
			ar >> lights.PerPixelLighting;
			ar >> lights.Alpha;
			SerializeLight(ar, lights.Light1);
			SerializeLight(ar, lights.Light2);
			SerializeLight(ar, lights.Light3);
		}
	}
}