#include "stdafx.h"
#include "VertexSource.h"

namespace DXF
{
	int VertexSourceToInt(VertexSource source)
	{
		if (source == VertexSource::Spherical)
			return 0;
		if (source == VertexSource::CustomVertexPair)
			return 1;

		return 0;
	}

	VertexSource VertexSourceFromInt(int val)
	{
		if (val == 0)
			return VertexSource::Spherical;
		if (val == 1)
			return VertexSource::CustomVertexPair;

		return VertexSource::Spherical;
	}
}