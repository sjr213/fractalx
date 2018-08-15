#pragma once

#include "SeedTriangles.h"

namespace DXF
{
	namespace VertexStatistics
	{
		int CalculateNumberOfVertices(int iterations, SeedTriangles seedTriangles);

		int CalculateNumberOfTriangles(int iterations, SeedTriangles seedTriangles);
	}
}
