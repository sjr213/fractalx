
#include "stdafx.h"
#include "BackgroundModelFactory.h"

#include <DirectXMath.h>
#include "RayTracerCommon.h"
#include "SphereApproximator.h"


using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DXF::BackgroundModelFactory
{
	std::vector<XMFLOAT3> CreateVertices()
	{
		std::vector<XMFLOAT3> seedVerices
		{
			XMFLOAT3(-2.0f, -2.0f, -2.0f),
			XMFLOAT3(-2.0f, -2.0f, 2.0f),
			XMFLOAT3(2.0f, -2.0f, 2.0f),
			XMFLOAT3(2.0f, -2.0f, -2.0f)
		};

		return seedVerices;
	}

	std::vector<Triangle> CreateTriangle()
	{
		std::vector<Triangle> seedTriangles
		{
			Triangle(0,1,2),
			Triangle(0,2,3)
		};

		return seedTriangles;
	}

	std::shared_ptr<TriangleData> GenerateCrudeTriangles()
	{
		std::shared_ptr<TriangleData> data = std::make_shared<TriangleData>();
		data->Vertices = CreateVertices();
		data->Triangles = CreateTriangle();

//		NormalizeVectors(*data);

		return data;
	}

	std::shared_ptr<DxVertexData> CreateModel()
	{
		auto triangleData = GenerateCrudeTriangles();

		std::shared_ptr<DxVertexData> vData = std::make_shared<DxVertexData>();

		XMFLOAT3 nullNormal(0.0f, 0.0f, 0.0f);
	//	XMFLOAT3 position(0.0f, 1.0f, 0.0f);

		vData->Vertices.emplace_back(triangleData->Vertices.at(0), nullNormal, Vector2(0.0f, 1.0f));
		vData->Vertices.emplace_back(triangleData->Vertices.at(1), nullNormal, Vector2(0.0f, 0.0f));
		vData->Vertices.emplace_back(triangleData->Vertices.at(2), nullNormal, Vector2(1.0f, 0.0f));
		vData->Vertices.emplace_back(triangleData->Vertices.at(3), nullNormal, Vector2(1.0f, 1.0f));

		for (const Triangle& t : triangleData->Triangles)
		{
			vData->Indices.push_back(t.one);
			vData->Indices.push_back(t.two);
			vData->Indices.push_back(t.three);
		}

		CalculateNormals(*vData);

		return vData;
	}

}