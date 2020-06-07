#pragma once

#include <iostream>
#include "vertex.h"

namespace DXF
{
	template<typename T>
	void Serialize(CArchive& ar, Vertex<T>& vertex)
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
	std::ostream& operator<<(std::ostream& output, const Vertex<T>& V) {
		output << V.X << V.Y << V.Z;
		return output;
	}

	template<typename T>
	std::istream& operator>>(std::istream& input, Vertex<T>& V) {
		input >> V.X >> V.Y >> V.Z;
		return input;
	}
}