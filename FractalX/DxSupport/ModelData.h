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
		Vertex<float> VertexTL;	// top left
		Vertex<float> VertexTR;	// top right
		Vertex<float> VertexBL;	// bottom left
		Vertex<float> VertexBR; // bottom right
	};

	void Serialize(CArchive& ar, ModelData& modelData);
}