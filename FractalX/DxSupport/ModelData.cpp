#include "stdafx.h"
#include "ModelData.h"
#include "VertexSerialization.h"
#include <assert.h>

namespace DXF
{
	// version 2 - added VertexSource and Vertex1 and Vertex2
	void SerializeModelData(CArchive& ar, ModelData& modelData)
	{
		const int ModelVersion = 2;

		if (ar.IsStoring())
		{
			ar << ModelVersion;
			ar << modelData.VertexIterations;
			ar << GetIndexForSeedTriangle(modelData.TriangleSeeds);
			ar << VertexSourceToInt(modelData.SourceVertices);
			SerializeVertex(ar, modelData.VertexTL);
			SerializeVertex(ar, modelData.VertexTR);
			SerializeVertex(ar, modelData.VertexBL);
			SerializeVertex(ar, modelData.VertexBR);
		}
		else
		{
			int version = 0;
			ar >> version;

			if (version < 1)
				return;

			ar >> modelData.VertexIterations;

			int seedType = 0;
			ar >> seedType;

			modelData.TriangleSeeds = GetSeedTriangleFromIndex(seedType);

			if (version < 2)
				return;

			int nVertexSource = 0;
			ar >> nVertexSource;
			modelData.SourceVertices = VertexSourceFromInt(nVertexSource);
			SerializeVertex(ar, modelData.VertexTL);
			SerializeVertex(ar, modelData.VertexTR);
			SerializeVertex(ar, modelData.VertexBL);
			SerializeVertex(ar, modelData.VertexBR);
		}
	}
}