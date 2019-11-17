#pragma once
#include <afx.h>

namespace DXF
{
	template<typename T>
	struct Vertex
	{
		Vertex()
			: X(0)
			, Y(0)
			, Z(0)
		{}

		Vertex(T x, T y, T z)
			: X(x)
			, Y(y)
			, Z(z)
		{}

		Vertex(const Vertex<T>& v)
			: X(v.X)
			, Y(v.Y)
			, Z(v.Z)
		{}

		T X;
		T Y;
		T Z;
	};

	template<typename T>
	void SerializeVertex(CArchive& ar, Vertex<T>& vertex)
	{
		if (ar.IsStoring())
		{
			ar << vertex.X << vertex.Y << vertex.Z;
		}
		else
		{
			ar >> vertex.X >> vertex.Y >> vertex.Z;
		}
	}

	template<typename T>
	Vertex<T> NormalizeVector(const Vertex<T>& v)
	{
		T sum = v.X * v.X + v.Y * v.Y + V.Z * v.Z;
		T norm = sqrt(sum);

		return Vertex<T>(v.X / norm, v.Y / norm, v.Z / norm);
	}

	template<typename T>
	bool operator==(const Vertex<T>& lf, const Vertex<T>& rt)
	{
		return lf.X == rt.X && lf.Y == rt.Y && lf.Z == rt.Z;
	}

	template<typename T>
	bool operator!=(const Vertex<T>& lf, const Vertex<T>& rt)
	{
		return !(lf == rt);
	}
}