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
		default:
			assert(false);
			return _T("Unknown");
		}
	}
}