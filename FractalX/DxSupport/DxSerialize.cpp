#include "stdafx.h"
#include "DxSerialize.h"
#include "VertexData.h"
#include "TraceParamsSerialize.h"

namespace DXF
{
	void Serialize(CArchive& ar, size_t n)
	{
		if (ar.IsStoring())
			ar << n;
		else
			ar >> n;
	}

	void Serialize(CArchive& ar, DirectX::XMFLOAT2& xf2)
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

	void Serialize(CArchive& ar, DirectX::VertexPositionNormalTexture& vpnt)
	{
		Serialize(ar, vpnt.position);
		Serialize(ar, vpnt.normal);
		Serialize(ar, vpnt.textureCoordinate);
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
}