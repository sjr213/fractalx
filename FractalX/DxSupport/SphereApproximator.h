#pragma once

#include <d3d11_1.h>
#include <memory>
#include <SimpleMath.h>
#include "SeedTriangles.h"
#include "VertexData.h"

namespace DXF
{
	// Each value corresponds to an index in the vertex vector
	struct Triangle
	{
		Triangle()
			: one(0)
			, two(0)
			, three(0)
		{}

		Triangle(unsigned int a, unsigned int b, unsigned int c)
			: one(a)
			, two(b)
			, three(c)
		{}

		unsigned int one;
		unsigned int two;
		unsigned int three;
	};

	struct TriangleData
	{
		std::vector<DirectX::XMFLOAT3> Vertices;
		std::vector<Triangle> Triangles;
	};

	std::shared_ptr<TriangleData> GenerateCrudeTriangles(int depth, SeedTriangles seeds, const std::function<void(double)>& setProgress);

	void NormalizeVector(const DirectX::XMFLOAT3& start, DirectX::XMFLOAT3& end);

	void NormalizeVectors(TriangleData& data);

	// input should be normalized first
	std::shared_ptr<DxVertexData> CreateModelFromSphereApproximation(const TriangleData& data);
}