#pragma once
#include "SeedTriangles.h"
#include "VertexSource.h"
#include "Vertex.h"

namespace DXF
{
	struct ModelData
	{
		int VertexIterations = 5;
		SeedTriangles TriangleSeeds = SeedTriangles::Eight;
		VertexSource SourceVertices = VertexSource::Spherical;
		Vertex<float> Vertex1;
		Vertex<float> Vertex2;
	};

	void SerializeModelData(CArchive& ar, ModelData& modelData);
}