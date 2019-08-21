#pragma once

namespace DXF
{
	enum class CartesianConversionType
	{
		StandardConversion = 1,
		CartesianConvertAltX1 = 2,
		CartesianConvertAltX2 = 3,
		CartesianConvertAltY1 = 4,
		CartesianConvertAltZ1 = 5
	};

	int CartesianConversionTypeToInt(CartesianConversionType fractalType);

	CartesianConversionType CartesianConversionTypeFromInt(int type);

	CString CartesianConversionTypeString(CartesianConversionType fractalType);
}