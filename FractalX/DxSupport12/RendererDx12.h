#pragma once

#include "Renderer.h"

namespace DXF
{
	std::unique_ptr<Renderer> CreateRendererDx12();
}