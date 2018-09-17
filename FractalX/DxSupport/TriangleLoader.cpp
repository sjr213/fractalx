#include "stdafx.h"
#include "TriangleLoader.h"

#include "ModelData.h"
#include "SphereApproximator.h"

namespace DXF
{
	namespace TriangleLoader
	{



		std::shared_ptr<TriangleData> GetTriangles(const ModelData& modelData, const std::function<void(double)>& setProgress)
		{
			std::shared_ptr<TriangleData> tData = GenerateCrudeTriangles(modelData.VertexIterations, modelData.TriangleSeeds, [&](double progress)
			{
				setProgress(progress);
			});

			NormalizeVectors(*tData);

			return tData;
		}
	}
}