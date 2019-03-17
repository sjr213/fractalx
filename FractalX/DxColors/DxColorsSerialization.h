
#pragma once

class CArchive;


namespace DxColor
{
	struct ColorArgb;
	struct ColorContrast;
	struct EffectColors;

	void SerializeColorArgb(CArchive& ar, ColorArgb& color);

	void Serialize(CArchive& ar, ColorContrast& contrast);

	void SerializeEffectColors(CArchive& ar, EffectColors& colors);
}