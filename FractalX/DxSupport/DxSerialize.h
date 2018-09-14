#pragma once

namespace DirectX
{
	struct XMFLOAT3;
}

namespace DXF
{

	struct DxVertexData;

	void Serialize(CArchive& ar, size_t n);

	template<typename T, typename Allocator>
	void Serialize(CArchive& ar, std::vector<T, Allocator>& Vector)
	{
		size_t nElements = Vector.size();
		Serialize(ar, nElements);

		Vector.resize(nElements);

		for (T& element : Vector)
			Serialize(ar, element);
	}

	void SerializeVertexData(CArchive& ar, DxVertexData& vextexData);

	void Serialize(CArchive& ar, DirectX::XMFLOAT3& xf3);
}
