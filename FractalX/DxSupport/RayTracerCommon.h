#pragma once

namespace DirectX { struct XMFLOAT3; }

namespace DirectX::SimpleMath { struct Vector3; }

namespace DXF
{
	struct DxVertexData;

	DirectX::XMFLOAT3 MakeStartingPoint(float distance, const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction);

	DirectX::XMFLOAT3 CrossProduct(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2);

	void CalculateNormals(DxVertexData& data);

}