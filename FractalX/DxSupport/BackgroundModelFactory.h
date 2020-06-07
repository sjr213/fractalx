#pragma once

#include "Vertex.h"

namespace DXF
{
	struct DxVertexData;
}

namespace DXF :: BackgroundModelFactory
{
	std::shared_ptr<DxVertexData> CreateModel(const std::vector<DXF::Vertex<float>>& modelCoordinates);
}