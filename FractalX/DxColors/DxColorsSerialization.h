
#pragma once

class CArchive;


namespace DxColor
{
	struct ColorArgb;
	struct ColorContrast;
	struct EffectColors;
	struct Light;
	struct Lights;

	void SerializeTuple3(CArchive& ar, std::tuple<float, float, float>& f3);

	void SerializeColorArgb(CArchive& ar, ColorArgb& color);

	void Serialize(CArchive& ar, ColorContrast& contrast);

	void SerializeEffectColors(CArchive& ar, EffectColors& colors);

	void SerializeLight(CArchive& ar, Light& light);

	void SerializeLights(CArchive& ar, Lights& lights);
}