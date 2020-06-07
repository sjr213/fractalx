
#include "stdafx.h"
#include "BackgroundModelFactory.h"

#include <DirectXMath.h>
#include "RayTracerCommon.h"
#include "SphereApproximator.h"


using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DXF::BackgroundModelFactory
{
	XMFLOAT3 VertexToXmfloat3(const DXF::Vertex<float>& v)
	{
		return XMFLOAT3(v.X, v.Y, v.Z);
	}

	std::vector<XMFLOAT3> CreateVertices(const std::vector<DXF::Vertex<float>>& modelCoordinates)
	{
		assert(modelCoordinates.size() == 4);
		std::vector<XMFLOAT3> seedVerices;
		if (modelCoordinates.size() != 4)
		{
			seedVerices.push_back(XMFLOAT3(-2.0f, -2.0f, -2.0f));
			seedVerices.push_back(XMFLOAT3(-2.0f, -2.0f, 2.0f));
			seedVerices.push_back(XMFLOAT3(2.0f, -2.0f, 2.0f));
			seedVerices.push_back(XMFLOAT3(2.0f, -2.0f, -2.0f));
		}
		else
		{
			seedVerices.push_back(VertexToXmfloat3(modelCoordinates.at(0)));
			seedVerices.push_back(VertexToXmfloat3(modelCoordinates.at(1)));
			seedVerices.push_back(VertexToXmfloat3(modelCoordinates.at(2)));
			seedVerices.push_back(VertexToXmfloat3(modelCoordinates.at(3)));
		}

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

	std::shared_ptr<TriangleData> GenerateCrudeTriangles(const std::vector<DXF::Vertex<float>>& modelCoordinates)
	{
		std::shared_ptr<TriangleData> data = std::make_shared<TriangleData>();
		data->Vertices = CreateVertices(modelCoordinates);
		data->Triangles = CreateTriangle();

		return data;
	}

	std::shared_ptr<DxVertexData> CreateModel(const std::vector<DXF::Vertex<float>>& modelCoordinates)
	{
		auto triangleData = GenerateCrudeTriangles(modelCoordinates);

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