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
		case FractalType::CartesianConvertAltX2:
			return 3;
		case FractalType::CartesianConvertAltY1:
			return 4;
		case FractalType::CartesianConvertAltZ1:
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
			return FractalType::CartesianConvertAltX1;
		case 3:
			return FractalType::CartesianConvertAltX2;
		case 4:
			return FractalType::CartesianConvertAltY1;
		case 5:
			return FractalType::CartesianConvertAltZ1;
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
		case FractalType::CartesianConvertAltX2:
			return _T("Bulb with Alt X2");
		case FractalType::CartesianConvertAltY1:
			return _T("Bulb with Alt Y1");
		case FractalType::CartesianConvertAltZ1:
			return _T("Bulb with Alt Z1");
		default:
			assert(false);
			return _T("Unknown");
		}
	}
}