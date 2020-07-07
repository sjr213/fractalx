#include "stdafx.h"
#include "ModelData.h"

#include <assert.h>
#include "..\DxCore\SerializationException.h"
#include "VertexSerialization.h"

namespace DXF
{
	// version 2 - added VertexSource and Vertex1 and Vertex2
	void Serialize(CArchive& ar, ModelData& modelData)
	{
		const int ModelVersion = 2;

		if (ar.IsStoring())
		{
			ar << ModelVersion;
			ar << modelData.VertexIterations;
			ar << GetIndexForSeedTriangle(modelData.TriangleSeeds);
			ar << VertexSourceToInt(modelData.SourceVertices);
			Serialize(ar, modelData.VertexTL);
			Serialize(ar, modelData.VertexTR);
			Serialize(ar, modelData.VertexBL);
			Serialize(ar, modelData.VertexBR);
		}
		else
		{
			int version = 0;
			ar >> version;

			if (version < 1)
				throw DxCore::CSerializationException("ModelData version less than 1");

			ar >> modelData.VertexIterations;

			int seedType = 0;
			ar >> seedType;

			modelData.TriangleSeeds = GetSeedTriangleFromIndex(seedType);

			if (version < 2)
				return;

			int nVertexSource = 0;
			ar >> nVertexSource;
			modelData.SourceVertices = VertexSourceFromInt(nVertexSource);
			Serialize(ar, modelData.VertexTL);
			Serialize(ar, modelData.VertexTR);
			Serialize(ar, modelData.VertexBL);
			Serialize(ar, modelData.VertexBR);
		}
	}
}