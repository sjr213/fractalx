#include "stdafx.h"
#include "ModelData.h"

namespace DXF
{
	void SerializeModelData(CArchive& ar, ModelData& modelData)
	{
		const int ModelVersion = 1;

		if (ar.IsStoring())
		{
			ar << ModelVersion;
			ar << modelData.VertexIterations;
			ar << GetIndexForSeedTriangle(modelData.TriangleSeeds);

		}
		else
		{
			int version = 0;
			ar >> version;

			assert(version == ModelVersion);
			if (version != ModelVersion)
				return;

			ar >> modelData.VertexIterations;

			int seedType = 0;
			ar >> seedType;

			modelData.TriangleSeeds = GetSeedTriangleFromIndex(seedType);
		}
	}
}