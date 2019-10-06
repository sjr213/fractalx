#include "stdafx.h"
#include "RayTracerCommon.h"

#include <SimpleMath.h>
#include "VertexData.h"
#include <DirectXMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace std;

namespace DXF
{
	static void SumFaceNormals(const XMFLOAT3& posA, const XMFLOAT3& posB, const XMFLOAT3& posC, XMVECTOR& normalA, XMVECTOR& normalB, XMVECTOR& normalC)
	{
		auto planeNormal = XMPlaneNormalize(XMPlaneFromPoints(
			XMLoadFloat3(&posA), XMLoadFloat3(&posB), XMLoadFloat3(&posC)));

		normalA += planeNormal * 2.0f;
		normalB += planeNormal;
		normalC += planeNormal;
	}

	XMFLOAT3 MakeStartingPoint(float distance, const XMFLOAT3& origin, const XMFLOAT3& direction)
	{
		return distance * direction + origin;
	}

	XMFLOAT3 CrossProduct(const XMFLOAT3& v1, const XMFLOAT3& v2)
	{
		Vector3 n(v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x);

		n.Normalize();

		return n;
	}

	// From Luna DirectX 11, 7.2.1
	void CalculateNormals(DxVertexData& data)
	{
		size_t nTriangles = data.Indices.size() / 3;

		for (size_t i = 0; i < nTriangles; ++i)
		{
			unsigned int i0 = data.Indices[i * 3 + 0];
			unsigned int i1 = data.Indices[i * 3 + 1];
			unsigned int i2 = data.Indices[i * 3 + 2];

			DirectX::VertexPositionNormalTexture v0 = data.Vertices[i0];
			DirectX::VertexPositionNormalTexture v1 = data.Vertices[i1];
			DirectX::VertexPositionNormalTexture v2 = data.Vertices[i2];

			// compute face normals
			DirectX::XMFLOAT3 e0 = v1.position - v0.position;
			DirectX::XMFLOAT3 e1 = v2.position - v0.position;
			DirectX::XMFLOAT3 faceNormal = CrossProduct(e0, e1);

			// This triangle shares the following three vertices.
			// so add this face normal into the average of these
			// vertex normals.
			data.Vertices[i0].normal = data.Vertices[i0].normal + faceNormal;
			data.Vertices[i1].normal = data.Vertices[i1].normal + faceNormal;
			data.Vertices[i2].normal = data.Vertices[i2].normal + faceNormal;
		}

		// normalize all vertex normals
		for (VertexPositionNormalTexture& vertex : data.Vertices)
		{
			Vector3 v = vertex.normal;
			v.Normalize();
			vertex.normal = v;
		}
	}

	// More common procedure like CT
	// does not look as sharp as CalculateNormals()
	void CalculateNormals2(DxVertexData& data)
	{
		auto& vertices = data.Vertices;
		vector<XMVECTOR> normals(vertices.size());
		auto& indices = data.Indices;

		for (size_t i = 0; i < indices.size(); i += 3)
		{
			auto index1 = indices[i];
			auto index2 = indices[i + 1];
			auto index3 = indices[i + 2];

			SumFaceNormals(vertices[index1].position, vertices[index2].position, vertices[index3].position,
				normals.at(index1), normals.at(index2), normals.at(index3));
		}

		auto currentVertex = vertices.begin();
		auto currentNormal = normals.begin();

		for (unsigned int i = 0; i < vertices.size(); ++i, ++currentVertex, ++currentNormal)
		{
			XMVector3Normalize(*currentNormal);
			XMStoreFloat3(&currentVertex->normal, *currentNormal);
		}
	}
}