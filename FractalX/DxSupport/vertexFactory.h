#pragma once

#include "ModelData.h"
#include "SeedTriangles.h"
#include "TraceParams.h"
#include "vector"
#include "VertexTypes.h"

namespace DXF
{
	struct DxVertexData;

	std::shared_ptr<DxVertexData> CreateData();

	std::shared_ptr<DxVertexData> CreateCubeData();

	std::shared_ptr<DxVertexData> CreateDoublePyramidData();

	std::shared_ptr<DxVertexData> CreateSphereModel(int n, SeedTriangles seeds, const std::function<void(double)>& setProgress);

	std::shared_ptr<DxVertexData> CreateBulb(ModelData modelData, TraceParams traceParams, 
		const std::function<void(double)>& setProgress);
}
