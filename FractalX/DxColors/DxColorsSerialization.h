
#pragma once

class CArchive;


namespace DxColor
{
	struct ColorArgb;
	struct ColorContrast;

	void SerializeColorArgb(CArchive& ar, ColorArgb& color);

	void Serialize(CArchive& ar, ColorContrast& contrast);
}