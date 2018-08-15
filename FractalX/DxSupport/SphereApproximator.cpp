
#include "stdafx.h"
#include "SphereApproximator.h"

#include <algorithm>


using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DXF
{

	static std::vector<Vector3> CreateSeedVertices(SeedTriangles seeds)
	{
		std::vector<Vector3> seedVerices
		{
			Vector3(0.0f, 1.0f, 0.0f),
			Vector3(1.0f, 0.0f, 0.0f),
			Vector3(0.0f, 0.0f, -1.0f)
		};

		if (seeds > SeedTriangles::One)
			seedVerices.push_back(Vector3(-1.0f, 0.0f, 0.0f));

		if (seeds > SeedTriangles::Two)
			seedVerices.push_back(Vector3(0.0f, 0.0f, 1.0f));

		if (seeds > SeedTriangles::Four)
			seedVerices.push_back(Vector3(0.0f, -1.0f, 0.0f));

		return seedVerices;
	}

	static std::vector<Triangle> CreateSeedTriangle(SeedTriangles seeds)
	{
		std::vector<Triangle> seedTriangles
		{
			Triangle(0,1,2),
		};

		if (seeds > SeedTriangles::One)
			seedTriangles.push_back(Triangle(0, 3, 2));

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

	Vector3 CalculateMidPoint(const Vector3& first, const Vector3& second)
	{
		return Vector3((first.x + second.x) / 2, (first.y + second.y) / 2, (first.z + second.z) / 2);
	}

	// try passing in the end of the old vertices to be more efficient
	unsigned int GetVertexIndex(std::vector<Vector3>& vertices, Vector3& vertex)
	{
		constexpr float minDif = 2 * std::numeric_limits<float>::min();

		auto iter = find_if(vertices.begin(), vertices.end(),
			[&](const Vector3& v)
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

	TriangleData ExpandTriangles(const TriangleData& data)
	{
		TriangleData newData;
		newData.Vertices = data.Vertices;

		// not sure if this will work
		//auto lastVertex = data.Vertices.end();

		// for each old triangle generate 4 new ones by creating 3 new vertices
		for (const Triangle& oldTriangle : data.Triangles)
		{
			// for each new vertex we need to determine if it already exist or if we should add it
			Vector3 newV1 = CalculateMidPoint(data.Vertices.at(oldTriangle.one), data.Vertices.at(oldTriangle.two));
			unsigned int newIndex1 = GetVertexIndex(newData.Vertices, newV1);

			Vector3 newV2 = CalculateMidPoint(data.Vertices.at(oldTriangle.two), data.Vertices.at(oldTriangle.three));
			unsigned int newIndex2 = GetVertexIndex(newData.Vertices, newV2);

			Vector3 newV3 = CalculateMidPoint(data.Vertices.at(oldTriangle.three), data.Vertices.at(oldTriangle.one));
			unsigned int newIndex3 = GetVertexIndex(newData.Vertices, newV3);

			newData.Triangles.emplace_back(oldTriangle.one, newIndex1, newIndex3);
			newData.Triangles.emplace_back(newIndex1, oldTriangle.two, newIndex2);
			newData.Triangles.emplace_back(newIndex3, newIndex2, oldTriangle.three);
			newData.Triangles.emplace_back(newIndex1, newIndex2, newIndex3);
		}

		return newData;
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

	TriangleData GenerateCrudeTriangles(int depth, SeedTriangles seeds, const std::function<void(double)>& setProgress)
	{
		int total = GetProgressSize(depth);
		total += 2;

		std::vector<Vector3> vertices = CreateSeedVertices(seeds);
		setProgress(1.0 / total);

		std::vector<Triangle> triangles = CreateSeedTriangle(seeds);
		setProgress(2.0 / total);

		TriangleData data{ vertices, triangles };

		for (int i = 0; i < depth; ++i)
		{
			data = ExpandTriangles(data);
			setProgress((3.0 + i) / total);
		}

		return data;
	}

	void NormalizeVector(const DirectX::SimpleMath::Vector3& start, DirectX::SimpleMath::Vector3& end)
	{
		end = end - start;

		end.Normalize();
	}

	void NormalizeVectors(TriangleData& data)
	{
		Vector3 origin(0, 0, 0);
		for (DirectX::SimpleMath::Vector3& v : data.Vertices)
		{
			NormalizeVector(origin, v);
		}
	}

	// input should be normalized first
	std::shared_ptr<DxVertexData> CreateModelFromSphereApproximation(const TriangleData& data)
	{
		std::shared_ptr<DxVertexData> vData = std::make_shared<DxVertexData>();

		for (const Vector3& v : data.Vertices)
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