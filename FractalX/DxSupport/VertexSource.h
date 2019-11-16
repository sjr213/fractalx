#pragma once

namespace DXF
{
	enum class VertexSource
	{
		Spherical = 0,
		CustomVertexPair
	};

	int VertexSourceToInt(VertexSource source);

	VertexSource VertexSourceFromInt(int val);

}