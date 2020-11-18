
#include "stdafx.h"
#include "SphereApproximator.h"

#include <algorithm>
#include <map>
#include "ModelData.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DXF
{
	// For vertex map methods
	namespace
	{
		struct cmp3D
		{
			bool operator()(const XMFLOAT3& lf, const XMFLOAT3& rt) const
			{
				if (lf.x != rt.x)
					return  lf.x < rt.x;
				if (lf.y != rt.y)
					return lf.y < rt.y;
				return lf.z < rt.z;
			}
		};

		using VertexIndex = uint32_t;
		using VertexMap = std::map<XMFLOAT3, VertexIndex, cmp3D>;

		VertexIndex GetVertexIndex(VertexMap& vertexMap, std::vector<DirectX::XMFLOAT3>& vertices, const XMFLOAT3& vertex)
		{
			constexpr float minDif = 2 * std::numeric_limits<float>::min();

			auto iter = vertexMap.find(vertex);

			if (iter == vertexMap.end())
			{
				vertices.push_back(vertex);
				VertexIndex newIndex = static_cast<VertexIndex>(vertices.size() - 1);
				vertexMap[vertex] = newIndex;
				return newIndex;
			}

			return iter->second;
		}

		XMFLOAT3 CalculateMidPoint(const XMFLOAT3& first, const XMFLOAT3& second)
		{
			return XMFLOAT3((first.x + second.x) / 2, (first.y + second.y) / 2, (first.z + second.z) / 2);
		}

		void ExpandTriangles(VertexMap& vertexMap, std::vector<DirectX::XMFLOAT3>& vertices, std::vector<Triangle>& triangles)
		{
			std::vector<Triangle> newTriangles;
			newTriangles.reserve(4 * triangles.size());

			// for each old triangle generate 4 new ones by creating 3 new vertices
			size_t nTriangles = triangles.size();
			for (size_t iTriangle = 0; iTriangle < nTriangles; ++iTriangle)
			{
				const Triangle& oldTriangle = triangles.at(iTriangle);

				// for each new vertex we need to determine if it already exist or if we should add it
				XMFLOAT3 newV1 = CalculateMidPoint(vertices.at(oldTriangle.one), vertices.at(oldTriangle.two));
				auto newIndex1 = GetVertexIndex(vertexMap, vertices, newV1);

				XMFLOAT3 newV2 = CalculateMidPoint(vertices.at(oldTriangle.two), vertices.at(oldTriangle.three));
				auto newIndex2 = GetVertexIndex(vertexMap, vertices, newV2);

				XMFLOAT3 newV3 = CalculateMidPoint(vertices.at(oldTriangle.three), vertices.at(oldTriangle.one));
				auto newIndex3 = GetVertexIndex(vertexMap, vertices, newV3);

				newTriangles.emplace_back(oldTriangle.one, newIndex1, newIndex3);
				newTriangles.emplace_back(newIndex1, oldTriangle.two, newIndex2);
				newTriangles.emplace_back(newIndex3, newIndex2, oldTriangle.three);
				newTriangles.emplace_back(newIndex1, newIndex2, newIndex3);
			}

			triangles = newTriangles;
		}

		VertexMap MakeVertexMap(const std::vector<DirectX::XMFLOAT3>& vertices)
		{
			VertexMap vertexMap;
			for (VertexIndex i = 0; i < vertices.size(); ++i)
			{
				vertexMap[vertices[i]] = i;
			}

			return vertexMap;
		}

		std::vector<XMFLOAT3> CreateSeedVertices(SeedTriangles seeds)
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

		std::vector<Triangle> CreateSeedTriangle(SeedTriangles seeds, int depth)
		{
			std::vector<Triangle> seedTriangles;

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

		XMFLOAT3 ConvertToXMFloat3(const Vertex<float>& v)
		{
			return XMFLOAT3(v.X, v.Y, v.Z);
		}

		int GetProgressSize(int depth)
		{
			int size = depth + 1;
			for (; depth > 0; --depth)
			{
				size += depth;
			}

			return size;
		}
	}

	std::shared_ptr<TriangleData> GenerateCrudeTriangles(int depth, SeedTriangles seeds, const std::function<void(double)>& setProgress)
	{
		int total = GetProgressSize(depth);
		total += 2;

		std::vector<XMFLOAT3> vertices = CreateSeedVertices(seeds);
		setProgress(1.0 / total);

		std::vector<Triangle> triangles = CreateSeedTriangle(seeds, depth);
		setProgress(2.0 / total);

		auto vertexMap = MakeVertexMap(vertices);

		for (int i = 0; i < depth; ++i)
		{
			ExpandTriangles(vertexMap, vertices, triangles);
			setProgress((3.0 + i) / total);
		}

		std::shared_ptr<TriangleData> data = std::make_shared<TriangleData>();
		data->Vertices = vertices;
		data->Triangles = triangles;

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

		auto vertexMap = MakeVertexMap(vertices);

		for (int i = 0; i < modelData.VertexIterations; ++i)
		{
			ExpandTriangles(vertexMap, vertices, triangles);
			setProgress((2.0 + i) / total);
		}

		std::shared_ptr<TriangleData> data = std::make_shared<TriangleData>();
		data->Vertices = vertices;
		data->Triangles = triangles;

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