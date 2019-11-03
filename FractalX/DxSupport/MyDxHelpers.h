#pragma once

#include <DirectXMath.h>
#include <optional>
#include <VertexTypes.h>

namespace DXF::DxHelpers
{
	bool GetIntersection(const std::vector<DirectX::VertexPositionNormalTexture>& vertices, const std::vector<unsigned int>& indices,
		const DirectX::XMVECTOR& clickTransform, DirectX::XMVECTOR& projectedVector, double& eClosestDistance);

	std::optional<std::tuple<float, float, float>> Create3DTuple(double closestDistance, 
		const DirectX::XMVECTOR& pointATransform, const DirectX::XMVECTOR& projectedVector);
}