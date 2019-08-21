#include "stdafx.h"
#include "CartesianConversionType.h"
#include <assert.h>

namespace DXF
{
	int CartesianConversionTypeToInt(CartesianConversionType conversionType)
	{
		switch (conversionType)
		{
		case CartesianConversionType::StandardConversion:
			return 1;
		case CartesianConversionType::CartesianConvertAltX1:
			return 2;
		case CartesianConversionType::CartesianConvertAltX2:
			return 3;
		case CartesianConversionType::CartesianConvertAltY1:
			return 4;
		case CartesianConversionType::CartesianConvertAltZ1:
			return 5;
		default:
			assert(false);
			return 1;
		}
	}

	CartesianConversionType CartesianConversionTypeFromInt(int type)
	{
		switch (type)
		{
		case 1:
			return CartesianConversionType::StandardConversion;
		case 2:
			return CartesianConversionType::CartesianConvertAltX1;
		case 3:
			return CartesianConversionType::CartesianConvertAltX2;
		case 4:
			return CartesianConversionType::CartesianConvertAltY1;
		case 5:
			return CartesianConversionType::CartesianConvertAltZ1;
		default:
			assert(false);
			return CartesianConversionType::StandardConversion;
		}
	}

	CString CartesianConversionTypeString(CartesianConversionType fractalType)
	{
		switch (fractalType)
		{
		case CartesianConversionType::StandardConversion:
			return _T("Standard Conversion");
		case CartesianConversionType::CartesianConvertAltX1:
			return _T("Bulb with Alt X1");
		case CartesianConversionType::CartesianConvertAltX2:
			return _T("Bulb with Alt X2");
		case CartesianConversionType::CartesianConvertAltY1:
			return _T("Bulb with Alt Y1");
		case CartesianConversionType::CartesianConvertAltZ1:
			return _T("Bulb with Alt Z1");
		default:
			assert(false);
			return _T("Unknown");
		}
	}
}