
#pragma once
#include "SimpleMath.h"

namespace DXF
{
	struct DxEffectColors
	{
		DxEffectColors()
			: AmbientColor(DirectX::SimpleMath::Vector4(0.24f, 0.24f, 0.24f, 1.0f))
			, DiffuseColor(DirectX::SimpleMath::Vector4(0.75f, 0.75f, 0.75f, 1.0f))
			, SpecularColor(DirectX::SimpleMath::Vector4(0.75f, 0.75f, 0.75f, 1.0f))
			, EmissiveColor(DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f))
			, SpecularPower(16.0f)
		{}

		DirectX::SimpleMath::Color AmbientColor;
		DirectX::SimpleMath::Color DiffuseColor;
		DirectX::SimpleMath::Color SpecularColor;
		DirectX::SimpleMath::Color EmissiveColor;
		float SpecularPower;
	};
}