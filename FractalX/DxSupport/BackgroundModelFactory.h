#pragma once

namespace DXF
{
	struct DxVertexData;
}

namespace DXF :: BackgroundModelFactory
{
	std::shared_ptr<DxVertexData> CreateModel();
}