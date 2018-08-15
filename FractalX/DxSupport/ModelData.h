#pragma once
#include "SeedTriangles.h"

namespace DXF
{
	struct ModelData
	{
		int VertexIterations = 5;
		SeedTriangles TriangleSeeds = SeedTriangles::Eight;
	};
}