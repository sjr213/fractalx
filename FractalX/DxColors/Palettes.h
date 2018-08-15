#pragma once
#include <memory>

namespace DxColor
{
	struct PinPalette;

	std::shared_ptr<PinPalette> GetRainbow();

	std::shared_ptr<PinPalette> GetStripes();
}