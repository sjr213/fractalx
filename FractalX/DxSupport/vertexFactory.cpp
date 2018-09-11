// Temporary factory methods

#include "stdafx.h"
#include "vertexFactory.h"

#include "BasicRayTracer.h"
#include <DirectXColors.h>
#include <SimpleMath.h>
#include "SphereApproximator.h"
#include "TraceParams.h"
#include "VertexData.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DXF
{
	std::shared_ptr<DxVertexData> CreateData()
	{
		std::shared_ptr<DxVertexData> vData = std::make_shared<DxVertexData>();

		vData->Vertices.emplace_back(Vector3(-0.5f, -0.5f, 0.0f), Vector3(0, 0, 1), Vector2(0.0f, 0.0f));	
		vData->Vertices.emplace_back(Vector3(0.0f, -0.5f, 0.0f), Vector3(0, 0, 1), Vector2(0.0f, 0.0f));
		vData->Vertices.emplace_back(Vector3(0.5f, -0.5f, 0.0f), Vector3(0, 0, 1), Vector2(0.0f, 0.0f));

		vData->Vertices.emplace_back(Vector3(-0.5f, 0.0f, 0.0f), Vector3(0, 0, 1), Vector2(0.5f, 0.0f));
		vData->Vertices.emplace_back(Vector3(0.0f, 0.0f, 0.0f), Vector3(0, 0, 1), Vector2(0.5f, 0.0f));
		vData->Vertices.emplace_back(Vector3(0.5f, 0.0f, 0.0f), Vector3(0, 0, 1), Vector2(0.5f, 0.0f));

		vData->Vertices.emplace_back(Vector3(-0.5f, 0.5f, 0.0f), Vector3(0, 0, 1), Vector2(1.0f, 0.0f));
		vData->Vertices.emplace_back(Vector3(0.0f, 0.5f, 0.0f), Vector3(0, 0, 1), Vector2(1.0f, 0.0f));
		vData->Vertices.emplace_back(Vector3(0.5f, 0.5f, 0.0f), Vector3(0, 0, 1), Vector2(1.0f, 0.0f));


		vData->Indices = { 0,1,3, 1,4,3, 1,2,4, 2,5,4, 3,4,6, 4,7,6, 4,5,7, 5,8,7 };

		return vData;
	}


	std::shared_ptr<DxVertexData> CreateCubeData()
	{
		std::shared_ptr<DxVertexData> vData = std::make_shared<DxVertexData>();

		vData->Vertices.emplace_back(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0, 0, 1), Vector2(0.0f, 0.0f));
		vData->Vertices.emplace_back(Vector3(0.5f, -0.5f, -0.5f), Vector3(0, 0, 1), Vector2(0.143f, 0.0f));
		vData->Vertices.emplace_back(Vector3(-0.5f, 0.5f, -0.5f), Vector3(0, 0, 1), Vector2(0.286f, 0.0f));
		vData->Vertices.emplace_back(Vector3(0.5f, 0.5f, -0.5f), Vector3(0, 0, 1), Vector2(0.432f, 0.0f));

		vData->Vertices.emplace_back(Vector3(-0.5f, -0.5f, 0.5f), Vector3(0, 0, 1), Vector2(0.571f, 0.0f));
		vData->Vertices.emplace_back(Vector3(0.5f, -0.5f, 0.5f), Vector3(0, 0, 1), Vector2(0.714f, 0.0f));
		vData->Vertices.emplace_back(Vector3(-0.5f, 0.5f, 0.5f), Vector3(0, 0, 1), Vector2(0.857f, 0.0f));
		vData->Vertices.emplace_back(Vector3(0.5f, 0.5f, 0.5f), Vector3(0, 0, 1), Vector2(1.0f, 0.0f));

		vData->Indices = { 0,1,2, 1,3,2, 4,5,6, 5,7,6, 4,0,6, 0,2,6, 4,5,0, 5,1,0, 1,5,3, 5,7,3, 2,3,6, 3,7,6};

		return vData;
	}

	std::shared_ptr<DxVertexData> CreateDoublePyramidData()
	{
		std::shared_ptr<DxVertexData> vData = std::make_shared<DxVertexData>();

		vData->Vertices.emplace_back(Vector3(0.0f, 1.0f, 0.0f), Vector3(0, 0, 1), Vector2(0.0f, 0.0f));
		vData->Vertices.emplace_back(Vector3(1.0f, 0.0f, 0.0f), Vector3(0, 0, 1), Vector2(0.2f, 0.0f));
		vData->Vertices.emplace_back(Vector3(0.0f, 0.0f, -1.0f), Vector3(0, 0, 1), Vector2(0.4f, 0.0f));
		vData->Vertices.emplace_back(Vector3(-1.0f, 0.0f, 0.0f), Vector3(0, 0, 1), Vector2(0.6f, 0.0f));
		vData->Vertices.emplace_back(Vector3(0.0f, 0.0f, 1.0f), Vector3(0, 0, 1), Vector2(0.8f, 0.0f));
		vData->Vertices.emplace_back(Vector3(0.0f, -1.0f, 0.0f), Vector3(0, 0, 1), Vector2(1.0f, 0.0f));

		vData->Indices = { 0,1,2, 0,2,3, 0,3,4, 0,4,1, 5,2,1, 5,3,2, 5,4,3, 5,1,4 };

		return vData;
	}

	std::shared_ptr<DxVertexData> CreateSphereModel(int n, SeedTriangles seeds, const std::function<void(double)>& setProgress)
	{
		TriangleData tData = GenerateCrudeTriangles(n, seeds, setProgress);
		NormalizeVectors(tData);

		return CreateModelFromSphereApproximation(tData);
	}

	std::shared_ptr<DxVertexData> CreateBulb(ModelData modelData, TraceParams traceParams, const std::function<void(double)>& setProgress)
	{
		TriangleData tData = GenerateCrudeTriangles(modelData.VertexIterations, modelData.TriangleSeeds, [&](double progress)
		{
			setProgress(progress / 3.0);
		});

		NormalizeVectors(tData);

		std::shared_ptr<BasicRayTracer> rayTracer = BasicRayTracer::CreateBasicRayTracer();

		if (traceParams.Stretch.StretchDistance)
		{
			return rayTracer->RayTraceStretch(tData, traceParams, [&](double progress)
			{
				setProgress(0.333 + 2.0*progress / 3.0);
			});
		}
		else
		{
			return rayTracer->RayTrace(tData, traceParams, [&](double progress)
			{
				setProgress(0.333 + 2.0*progress / 3.0);
			});
		}
	}

}