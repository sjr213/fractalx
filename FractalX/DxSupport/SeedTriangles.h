#pragma once


namespace DXF
{
	enum class SeedTriangles
	{
		One = 1,
		Two = 2,
		Four = 4,
		Eight = 8
	};

	CString GetSeedTriangleString(SeedTriangles seedTriangles);

	SeedTriangles GetSeedTriangleFromIndex(int index);

	int GetIndexForSeedTriangle(SeedTriangles seedTriangles);
}