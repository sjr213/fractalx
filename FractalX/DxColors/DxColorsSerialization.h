
#pragma once

#include "EffectColors.h"

class CArchive;

namespace DxColor
{
	struct ColorArgb;
	struct ColorContrast;
	struct Light;
	struct Lights;

	void SerializeTuple3(CArchive& ar, std::tuple<float, float, float>& f3);

	void Serialize(CArchive& ar, ColorArgb& color);

	void Serialize(CArchive& ar, ColorContrast& contrast);

	void Serialize(CArchive& ar, EffectColors& colors, int version);

	void Serialize(CArchive& ar, Light& light);

	void Serialize(CArchive& ar, Lights& lights);
}