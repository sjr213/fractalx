#pragma once

namespace DXF
{
	enum class Ingles3EquationType
	{
		I_Squared = 1,
		I_Cubed = 2,
		I_SinX = 3
	};

	int Ingles3EquationTypeToInt(Ingles3EquationType ingles3Type);

	Ingles3EquationType Ingles3EquationTypeFromInt(int type);

	CString Ingles3EquationTypeString(Ingles3EquationType ingles3Type);
}