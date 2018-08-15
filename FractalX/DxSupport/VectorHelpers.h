#pragma once
#include <SimpleMath.h>

namespace DXF
{
	DirectX::SimpleMath::Vector3 AddScaler(const DirectX::SimpleMath::Vector3& v, float s)
	{
		DirectX::SimpleMath::Vector3 outV = v;
		outV.x += s;
		outV.y += s;
		outV.z += s;

		return outV;
	}
}
