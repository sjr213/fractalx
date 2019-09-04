#include "stdafx.h"
#include "RayTracerCommon.h"

#include <SimpleMath.h>
#include "VertexData.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DXF
{
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
	/*
	Vector3 AddScaler(const Vector3& v, float s)
	{
		Vector3 outV = v;
		outV.x += s;
		outV.y += s;
		outV.z += s;

		return outV;
	}
	*/
}