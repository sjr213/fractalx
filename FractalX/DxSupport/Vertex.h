#pragma once
//#include <afx.h>

namespace DXF
{
	template<typename T>
	struct Vertex
	{
		T X;
		T Y;
		T Z;

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
	};


	template<typename T>
	Vertex<T> NormalizeVector(const Vertex<T>& v)
	{
		T sum = v.X * v.X + v.Y * v.Y + v.Z * v.Z;
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

	template<typename T>
	bool AreVerticesSimilar(const Vertex<T>& lf, const Vertex<T>& rt, T maxDifference)
	{
		return fabs(lf.X - rt.X) < maxDifference
			&& fabs(lf.Y - rt.Y) < maxDifference
			&& fabs(lf.Z - rt.Z) < maxDifference;
	}

}