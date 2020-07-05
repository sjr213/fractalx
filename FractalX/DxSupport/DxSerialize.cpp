#include "stdafx.h"
#include "DxSerialize.h"

#include "BackgroundVertexData.h"
#include "..\DxCore\SerializationException.h"
#include "Perspective.h"
#include "RotationGroup.h"
#include "RotationParams.h"
#include "SphereApproximator.h"
#include "TraceParamsSerialize.h"
#include "VertexData.h"

namespace DXF
{
	bool IsVersionSupported(int version, int minVersion, int maxVersion)
	{
		return (version >= minVersion && version <= maxVersion);
	}

	void Serialize(CArchive& ar, bool& val)
	{
		int num = val;
		if (ar.IsStoring())
			ar << num;
		else
		{
			ar >> num;
			val = static_cast<bool>(num);
		}
	}

	void Serialize(CArchive& ar, size_t& n)
	{
		if (ar.IsStoring())
			ar << n;
		else
			ar >> n;
	}

	void Serialize(CArchive& ar, std::wstring& str)
	{
		CString cstr = str.c_str();
		if (ar.IsStoring())
			ar << cstr;
		else
		{
			ar >> cstr;
			str = cstr.GetString();
		}
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
				throw DxCore::CSerializationException("VertexData version not supported");
		}

		Serialize(ar, vertexData.Vertices);
		Serialize(ar, vertexData.Indices);
		Serialize(ar, vertexData.StretchParams);
	}

	void Serialize(CArchive& ar, DxBackgroundVertexData& bkgndVertexData)
	{
		const int BkgndVertexVersion = 1;

		if (!bkgndVertexData.VertexData)
			throw DxCore::CSerializationException("Background vertex missing");

		if (ar.IsStoring())
		{
			ar << BkgndVertexVersion;
		}
		else
		{
			int version = 0;
			ar >> version;

			if (version != BkgndVertexVersion)
				throw DxCore::CSerializationException("Background vertex version less than 1");
		}

		Serialize(ar, bkgndVertexData.Show);
		SerializeVertexData(ar, *bkgndVertexData.VertexData);
		Serialize(ar, bkgndVertexData.Filename);
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

			if (version != TriangleVersion)
				throw DxCore::CSerializationException("Triangle data version less than 1");
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

			if (version != RotationParams::RotationVersion)
				throw DxCore::CSerializationException("Rotation param version less than 1");

			int action = 0;
			ar >> action;
			rotationParams.Action = RotationActionFromInt(action);
			ar >> rotationParams.AngleXDegrees >> rotationParams.AngleYDegrees >> rotationParams.AngleZDegrees;
		}
	}

	void Serialize(CArchive& ar, std::tuple<float, float, float>& f3)
	{
		if (ar.IsStoring())
		{
			ar << std::get<0>(f3) << std::get<1>(f3) << std::get<2>(f3);
		}
		else
		{
			ar >> std::get<0>(f3) >> std::get<1>(f3) >> std::get<2>(f3);
		}
	}

	void Serialize(CArchive& ar, std::tuple<double, double, double>& d3)
	{
		if (ar.IsStoring())
		{
			ar << std::get<0>(d3) << std::get<1>(d3) << std::get<2>(d3);
		}
		else
		{
			ar >> std::get<0>(d3) >> std::get<1>(d3) >> std::get<2>(d3);
		}
	}

	void Serialize(CArchive& ar, DxPerspective& perspective)
	{
		if (ar.IsStoring())
		{
			ar << perspective.NearPlane << perspective.FarPlane;
		}
		else
		{
			ar >> perspective.NearPlane >> perspective.FarPlane;
		}
	}

	void Serialize(CArchive& ar, RotationGroup& rotationGroup)
	{
		int version = RotationGroup::RotationGroupVersion;

		if (ar.IsStoring())
		{
			ar << version;
			ar << RotationSelectionTypeToInt(rotationGroup.RotationType);
			Serialize(ar, rotationGroup.RotationParamsMain);
			Serialize(ar, rotationGroup.RotationParamsBackground);
		}
		else
		{
			int version = 0;
			ar >> version;

			if (version != RotationGroup::RotationGroupVersion)
				throw DxCore::CSerializationException("Rotation group version less than 1");

			int type = 0;
			ar >> type;
			rotationGroup.RotationType = RotationSelectionTypeFromInt(type);
			Serialize(ar, rotationGroup.RotationParamsMain);
			Serialize(ar, rotationGroup.RotationParamsBackground);
		}
	}
}