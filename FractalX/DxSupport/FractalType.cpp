#include "stdafx.h"
#include "FractalType.h"

#include <assert.h>

namespace DXF
{
	int FractalTypeToInt(FractalType fractalType)
	{
		switch (fractalType)
		{
		case FractalType::StandardBulb:
			return 1;
		case FractalType::DoubleBulb:
			return 2;
		case FractalType::InglesFractal:
			return 3;
		case FractalType::InglesFractal2:
			return 4;
		case FractalType::InglesFractal3:
			return 5;
		default:
			assert(false);
			return 1;
		}
	}

	FractalType FractalTypeFromInt(int type)
	{
		switch (type)
		{
		case 1:
			return FractalType::StandardBulb;
		case 2:
			return FractalType::DoubleBulb;
		case 3:
			return FractalType::InglesFractal;
		case 4:
			return FractalType::InglesFractal2;
		case 5:
			return FractalType::InglesFractal3;
		default:
			assert(false);
			return FractalType::StandardBulb;
		}
	}

	CString FractalTypeString(FractalType fractalType)
	{
		switch (fractalType)
		{
		case FractalType::StandardBulb:
			return _T("Standard Bulb");
		case FractalType::DoubleBulb:
			return _T("Double Bulb");
		case FractalType::InglesFractal:
			return _T("Ingles");
		case FractalType::InglesFractal2:
			return _T("Ingles2");
		case FractalType::InglesFractal3:
			return _T("Ingles3");
		default:
			assert(false);
			return _T("Unknown");
		}
	}
}