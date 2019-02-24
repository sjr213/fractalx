#include "stdafx.h"
#include "DxSerialize.h"

#include "SphereApproximator.h"
#include "TraceParamsSerialize.h"
#include "VertexData.h"
#include "RotationParams.h"


namespace DXF
{
	void Serialize(CArchive& ar, size_t& n)
	{
		if (ar.IsStoring())
			ar << n;
		else
			ar >> n;
	}

	void Serialize(CArchive& ar, unsigned int& n)
	{
		if (ar.IsStoring())
			ar << n;
		else
			ar >> n;
	}

	static void Serialize(CArchive& ar, DirectX::XMFLOAT2& xf2)
	{
		if (ar.IsStoring())
		{
			ar << xf2.x << xf2.y;
		}
		else
		{
			ar >> xf2.x >> xf2.y;
		}
	}

	void Serialize(CArchive& ar, DirectX::XMFLOAT3& xf3)
	{
		if (ar.IsStoring())
		{
			ar << xf3.x << xf3.y << xf3.z;
		}
		else
		{
			ar >> xf3.x >> xf3.y >> xf3.z;
		}
	}

	static void Serialize(CArchive& ar, DirectX::VertexPositionNormalTexture& vpnt)
	{
		Serialize(ar, vpnt.position);
		Serialize(ar, vpnt.normal);
		Serialize(ar, vpnt.textureCoordinate);
	}

	static void Serialize(CArchive& ar, Triangle& triangle)
	{
		if (ar.IsStoring())
		{
			ar << triangle.one << triangle.two << triangle.three;
		}
		else
		{
			ar >> triangle.one >> triangle.two >> triangle.three;
		}
	}

	void SerializeVertexData(CArchive& ar, DxVertexData& vertexData)
	{
		const int VertexVersion = 1;

		if (ar.IsStoring())
		{
			ar << VertexVersion;
		}
		else
		{
			int version = 0;
			ar >> version;

			assert(version == VertexVersion);
			if (version != VertexVersion)
				return;
		}

		Serialize(ar, vertexData.Vertices);
		Serialize(ar, vertexData.Indices);
		Serialize(ar, vertexData.StretchParams);
	}

	void Serialize(CArchive& ar, TriangleData& triangles)
	{
		const int TriangleVersion = 1;

		if (ar.IsStoring())
		{
			ar << TriangleVersion;
		}
		else
		{
			int version = 0;
			ar >> version;

			assert(version == TriangleVersion);
			if (version != TriangleVersion)
				return;
		}

		Serialize(ar, triangles.Triangles);
		Serialize(ar, triangles.Vertices);
	}

	void Serialize(CArchive& ar, RotationParams& rotationParams)
	{
		int version = RotationParams::RotationVersion;

		if (ar.IsStoring())
		{
			ar << version;
			ar << RotationActionToInt(rotationParams.Action);
			ar << rotationParams.AngleXDegrees << rotationParams.AngleYDegrees << rotationParams.AngleZDegrees;
		}
		else
		{
			int version = 0;
			ar >> version;

			assert(version == RotationParams::RotationVersion);
			if (version != RotationParams::RotationVersion)
				return;

			int action = 0;
			ar >> action;
			rotationParams.Action = RotationActionFromInt(action);
			ar >> rotationParams.AngleXDegrees >> rotationParams.AngleYDegrees >> rotationParams.AngleZDegrees;
		}




	}
}