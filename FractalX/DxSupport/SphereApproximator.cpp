
#include "stdafx.h"
#include "SphereApproximator.h"

#include <algorithm>
#include "ModelData.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DXF
{

	static std::vector<XMFLOAT3> CreateSeedVertices(SeedTriangles seeds)
	{
		std::vector<XMFLOAT3> seedVerices
		{
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT3(0.0f, 0.0f, -1.0f)
		};

		if (seeds > SeedTriangles::One)
			seedVerices.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));

		if (seeds > SeedTriangles::Two)
			seedVerices.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));

		if (seeds > SeedTriangles::Four)
			seedVerices.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));

		return seedVerices;
	}

	static unsigned int GetTriangleSize(SeedTriangles seeds, int depth)
	{
		unsigned int seedSize = 0;
		switch (seeds)
		{
		case SeedTriangles::One:
			seedSize = 1;
			break;
		case SeedTriangles::Two:
			seedSize = 2;
			break;
		case SeedTriangles::Four:
			seedSize = 4;
			break;
		case SeedTriangles::Eight:
			seedSize = 8;
			break;
		}

		for (int i = 0; i < depth; ++i)
			seedSize *= 4;

		return seedSize;
	}

	static std::vector<Triangle> CreateSeedTriangle(SeedTriangles seeds, int depth)
	{
		std::vector<Triangle> seedTriangles;
		auto nTriangles = GetTriangleSize(seeds, depth);
		seedTriangles.reserve(nTriangles);

		seedTriangles.push_back(Triangle(0, 1, 2));

		if (seeds > SeedTriangles::One)
			seedTriangles.push_back(Triangle(0, 2, 3));

		if (seeds > SeedTriangles::Two)
		{
			seedTriangles.push_back(Triangle(0, 3, 4));
			seedTriangles.push_back(Triangle(0, 4, 1));
		}

		if (seeds > SeedTriangles::Four)
		{
			seedTriangles.push_back(Triangle(5, 2, 1));
			seedTriangles.push_back(Triangle(5, 3, 2));
			seedTriangles.push_back(Triangle(5, 4, 3));
			seedTriangles.push_back(Triangle(5, 1, 4));		
		}

		return seedTriangles;
	}

	static XMFLOAT3 CalculateMidPoint(const XMFLOAT3& first, const XMFLOAT3& second)
	{
		return XMFLOAT3((first.x + second.x) / 2, (first.y + second.y) / 2, (first.z + second.z) / 2);
	}

	// try passing in the end of the old vertices to be more efficient
	static unsigned int GetVertexIndex(std::vector<XMFLOAT3>& vertices, XMFLOAT3& vertex)
	{
		constexpr float minDif = 2 * std::numeric_limits<float>::min();

		auto iter = find_if(vertices.begin(), vertices.end(),
			[&](const XMFLOAT3& v)
			{
				return fabs(v.x - vertex.x) < minDif &&
						fabs(v.y - vertex.y) < minDif &&
						fabs(v.z - vertex.z) < minDif;
			}
		);

		if (iter == vertices.end())
		{
			vertices.push_back(vertex);
			return static_cast<unsigned int>(vertices.size() - 1);
		}

		return static_cast<unsigned int>(iter - vertices.begin());
	}

	static std::shared_ptr<TriangleData> ExpandTriangles(const TriangleData& data)
	{
		std::shared_ptr<TriangleData> newData = std::make_shared<TriangleData>();
		newData->Vertices = data.Vertices;

		// not sure if this will work
		//auto lastVertex = data.Vertices.end();

		// for each old triangle generate 4 new ones by creating 3 new vertices
		size_t nTriangles = data.Triangles.size();
		for (size_t iTriangle = 0; iTriangle < nTriangles; ++iTriangle)
		{
			const Triangle& oldTriangle = data.Triangles.at(iTriangle);

			// for each new vertex we need to determine if it already exist or if we should add it
			XMFLOAT3 newV1 = CalculateMidPoint(data.Vertices.at(oldTriangle.one), data.Vertices.at(oldTriangle.two));
			unsigned int newIndex1 = GetVertexIndex(newData->Vertices, newV1);

			XMFLOAT3 newV2 = CalculateMidPoint(data.Vertices.at(oldTriangle.two), data.Vertices.at(oldTriangle.three));
			unsigned int newIndex2 = GetVertexIndex(newData->Vertices, newV2);

			XMFLOAT3 newV3 = CalculateMidPoint(data.Vertices.at(oldTriangle.three), data.Vertices.at(oldTriangle.one));
			unsigned int newIndex3 = GetVertexIndex(newData->Vertices, newV3);

			newData->Triangles.emplace_back(oldTriangle.one, newIndex1, newIndex3);
			newData->Triangles.emplace_back(newIndex1, oldTriangle.two, newIndex2);
			newData->Triangles.emplace_back(newIndex3, newIndex2, oldTriangle.three);
			newData->Triangles.emplace_back(newIndex1, newIndex2, newIndex3);
		}

		return newData;
	}

	static XMFLOAT3 ConvertToXMFloat3(const Vertex<float>& v)
	{
		return XMFLOAT3(v.X, v.Y, v.Z);
	}

	static int GetProgressSize(int depth)
	{
		int size = depth + 1;
		for (; depth > 0; --depth)
		{
			size += depth;
		}

		return size;
	}

	std::shared_ptr<TriangleData> GenerateCrudeTriangles(int depth, SeedTriangles seeds, const std::function<void(double)>& setProgress)
	{
		int total = GetProgressSize(depth);
		total += 2;

		std::vector<XMFLOAT3> vertices = CreateSeedVertices(seeds);
		setProgress(1.0 / total);

		std::vector<Triangle> triangles = CreateSeedTriangle(seeds, depth);
		setProgress(2.0 / total);

		std::shared_ptr<TriangleData> data = std::make_shared<TriangleData>();
		data->Vertices = vertices;
		data->Triangles = triangles;

		for (int i = 0; i < depth; ++i)
		{
			data = ExpandTriangles(*data);
			setProgress((3.0 + i) / total);
		}

		return data;
	}

	std::shared_ptr<TriangleData> GenerateTrianglesFromCrudeVertices(const ModelData& modelData, const std::function<void(double)>& setProgress)
	{
		int total = GetProgressSize(modelData.VertexIterations);
		total += 1;	// 1 for the vertices and triangles

		// vertices
		std::vector<XMFLOAT3> vertices;
		vertices.push_back(ConvertToXMFloat3(modelData.VertexTL));  // TL
		vertices.push_back(ConvertToXMFloat3(modelData.VertexTR));  // TR
		vertices.push_back(ConvertToXMFloat3(modelData.VertexBR));  // BR
		vertices.push_back(ConvertToXMFloat3(modelData.VertexBL));  // BL

		// triangles
		std::vector<Triangle> triangles
		{
			Triangle(0,2,1),
			Triangle(0,3,2)
		};

		setProgress(1.0 / total);

		// expand
		std::shared_ptr<TriangleData> data = std::make_shared<TriangleData>();
		data->Vertices = vertices;
		data->Triangles = triangles;

		for (int i = 0; i < modelData.VertexIterations; ++i)
		{
			data = ExpandTriangles(*data);
			setProgress((2.0 + i) / total);
		}

		return data;
	}

	void NormalizeVector(const XMFLOAT3& start, XMFLOAT3& end)
	{
		Vector3 newEnd = end - start;

		newEnd.Normalize();

		end = newEnd;
	}

	void NormalizeVectors(TriangleData& data)
	{
		XMFLOAT3 origin(0, 0, 0);
		for (XMFLOAT3& v : data.Vertices)
		{
			NormalizeVector(origin, v);
		}
	}

	// input should be normalized first
	std::shared_ptr<DxVertexData> CreateModelFromSphereApproximation(const TriangleData& data)
	{
		std::shared_ptr<DxVertexData> vData = std::make_shared<DxVertexData>();

		for (const XMFLOAT3& v : data.Vertices)
		{
			float t0 = (1.0f + v.y) / 2.0f;
			vData->Vertices.emplace_back(v, v, Vector2(t0, 0.0f));
		}

		for (const Triangle& t : data.Triangles)
		{
			vData->Indices.push_back(t.one);
			vData->Indices.push_back(t.two);
			vData->Indices.push_back(t.three);
		}

		return vData;
	}

}