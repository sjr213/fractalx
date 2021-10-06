#include "stdafx.h"
#include "Dx12CoordinateMap.h"

using namespace DXF;

namespace DxSupport::Dx12CoordinateMap
{
	Vertex<float> ToD12Target(const Vertex<float>& target)
	{
		Vertex<float> d12Target(-target.X, target.Y, target.Z);
		return d12Target;
	}

	Vertex<float> FromD12Target(const Vertex<float>& target)
	{
		return ToD12Target(target);
	}

	RotationParams ToD12RotationParams(const RotationParams& rt)
	{
		auto d12Rt = rt;
		d12Rt.AngleZDegrees = 180 - d12Rt.AngleZDegrees;
		d12Rt.AngleXDegrees = -1 * d12Rt.AngleXDegrees;

		return d12Rt;
	}

	RotationParams FromD12RotationParams(const RotationParams& rt)
	{
		auto d11Rt = rt;
		d11Rt.AngleZDegrees = 180 - d11Rt.AngleZDegrees;
		d11Rt.AngleXDegrees = -1 * d11Rt.AngleXDegrees;

		return d11Rt;
	}

	RotationGroup ToD12RotationGroup(const RotationGroup& rg)
	{
		RotationGroup rg12;
		rg12.RotationType = rg.RotationType;
		rg12.RotationParamsMain = ToD12RotationParams(rg.RotationParamsMain);
		rg12.RotationParamsBackground = ToD12RotationParams(rg.RotationParamsBackground);

		return rg12;
	}

	DXF::RotationGroup FromD12RotationGroup(const DXF::RotationGroup& rg)
	{
		RotationGroup rg11;
		rg11.RotationType = rg.RotationType;
		rg11.RotationParamsMain = FromD12RotationParams(rg.RotationParamsMain);
		rg11.RotationParamsBackground = FromD12RotationParams(rg.RotationParamsBackground);

		return rg11;
	}
}