#include "stdafx.h"
#include "TraceParams.h"

namespace DXF
{
	int FractalTypeToInt(FractalType fractalType)
	{
		switch (fractalType)
		{
		case FractalType::StandardBulb:
			return 1;
		case FractalType::CartesianConvertAltX1:
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
			return FractalType::CartesianConvertAltX1;
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
		case FractalType::CartesianConvertAltX1:
			return _T("Bulb with Alt X1");
		default:
			assert(false);
			return _T("Unknown");
		}
	}
}