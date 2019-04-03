#pragma once

#include "SimpleMath.h"

namespace DXF
{
	struct DxLight
	{
		DxLight()
			: Enable(false)
			, DiffuseColor(DirectX::SimpleMath::Vector4(0.75f, 0.75f, 0.75f, 1.0f))
			, SpectacularColor(DirectX::SimpleMath::Vector4(0.75f, 0.75f, 0.75f, 1.0f))
			, Direction(0.0f, 0.0f, -1.0f)
		{}

		bool Enable;
		DirectX::SimpleMath::Color DiffuseColor;
		DirectX::SimpleMath::Color SpectacularColor;
		DirectX::SimpleMath::Vector3 Direction;
	};

	struct DxLights
	{
		DxLights()
			: DefaultLights(false)
			, PerPixelLighting(false)
			, Alpha(1.0f)
			, Lights(3)
		{}

		bool DefaultLights;
		bool PerPixelLighting;
		float Alpha;

		std::vector<DxLight> Lights;
	};
}