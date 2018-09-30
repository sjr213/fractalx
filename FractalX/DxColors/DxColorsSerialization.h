#pragma once

class CArchive;

namespace DxColor
{
	struct ColorContrast;

	namespace Serialization
	{
		void Serialize(CArchive& ar, ColorContrast& contrast);
	}
}