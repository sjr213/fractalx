#pragma once

namespace DXF
{
	enum class FractalType
	{
		StandardBulb = 1,
		DoubleBulb = 2
	};

	int FractalTypeToInt(FractalType fractalType);

	FractalType FractalTypeFromInt(int type);

	CString FractalTypeString(FractalType fractalType);
}
