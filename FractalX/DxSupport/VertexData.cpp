#include "stdafx.h"
#include "VertexData.h"

namespace DXF
{
	// need to serialize these for each DirectX::VertexPositionNormalTexture
	// XMFLOAT3 position;
	// XMFLOAT3 normal;
	// XMFLOAT2 textureCoordinate;

	/* struct XMFLOAT3
	{
		float x;
		float y;
		float z;
	}

	struct XMFLOAT2
	{
	float x;
	float y;
	}

	*/

	void SerializeVertexData(CArchive& ar, DxVertexData& vextexData)
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


	}
}