#pragma once

#include "vertex.h"

namespace DXF
{
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
}