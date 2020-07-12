#pragma once

#include "DxEffectColors.h"
#include "DxLight.h"

namespace DxColor 
{ 
	struct EffectColors; 
	struct Light;
	struct Lights;
}

namespace ConversionUtil
{
	DXF::DxEffectColors ConvertEffectColors(const DxColor::EffectColors& effectColors);

	DXF::DxLights ConvertLights(const DxColor::Lights& lights);

	CRect MakeRect(CPoint pt1, CPoint pt2);
}
