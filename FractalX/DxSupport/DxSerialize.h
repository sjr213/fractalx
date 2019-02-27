#pragma once

namespace DirectX
{
	struct XMFLOAT3;
}

namespace DXF
{

	struct DxVertexData;
	struct TriangleData;
	struct RotationParams;
	struct DxPerspective;

	void Serialize(CArchive& ar, size_t& n);

	void Serialize(CArchive& ar, unsigned int& n);

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

	void Serialize(CArchive& ar, TriangleData& triangles);

	void Serialize(CArchive& ar, RotationParams& rotationParams);

	void Serialize(CArchive& ar, std::tuple<float, float, float>& f3);

	void Serialize(CArchive& ar, DxPerspective& perspective);
}
