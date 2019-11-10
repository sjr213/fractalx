#include "stdafx.h"
#include "SeedTriangles.h"

#include <assert.h>

namespace DXF
{
	CString GetSeedTriangleString(SeedTriangles seedTriangles)
	{
		switch (seedTriangles)
		{
		case SeedTriangles::One:
			return _T("1");
		case SeedTriangles::Two:
			return _T("2");
		case SeedTriangles::Four:
			return _T("4");
		case SeedTriangles::Eight:
			return _T("8");
		default:
			assert(false);
			return _T("");
		}
	}

	SeedTriangles GetSeedTriangleFromIndex(int index)
	{
		switch (index)
		{
		case 0:
			return SeedTriangles::One;
		case 1:
			return SeedTriangles::Two;
		case 2:
			return SeedTriangles::Four;
		case 3:
			return SeedTriangles::Eight;
		default:
			assert(false);
			return SeedTriangles::Eight;
		}
	}

	int GetIndexForSeedTriangle(SeedTriangles seedTriangles)
	{
		switch (seedTriangles)
		{
		case SeedTriangles::One:
			return 0;
		case SeedTriangles::Two:
			return 1;
		case SeedTriangles::Four:
			return 2;
		case SeedTriangles::Eight:
			return 3;
		default:
			assert(false);
			return 3;
		}
	}
}