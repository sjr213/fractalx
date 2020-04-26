#include "stdafx.h"
#include "Ingles3EquantionType.h"

#include <assert.h>

namespace DXF
{
	int Ingles3EquationTypeToInt(Ingles3EquationType ingles3Type)
	{
		switch (ingles3Type)
		{
		case Ingles3EquationType::I_Squared:
			return 1;
		case Ingles3EquationType::I_Cubed:
			return 2;
		case Ingles3EquationType::I_SinX:
			return 3;
		case Ingles3EquationType::I_Alt1:
			return 4;
		case Ingles3EquationType::I_Alt2:
			return 5;
		default:
			assert(false);
			return 1;
		}
	}

	Ingles3EquationType Ingles3EquationTypeFromInt(int type)
	{
		switch (type)
		{
		case 1:
			return Ingles3EquationType::I_Squared;
		case 2:
			return Ingles3EquationType::I_Cubed;
		case 3:
			return Ingles3EquationType::I_SinX;
		case 4:
			return Ingles3EquationType::I_Alt1;
		case 5:
			return Ingles3EquationType::I_Alt2;
		default:
			assert(false);
			return Ingles3EquationType::I_Squared;
		}
	}

	CString Ingles3EquationTypeString(Ingles3EquationType ingles3Type)
	{
		switch (ingles3Type)
		{
		case Ingles3EquationType::I_Squared:
			return _T("Ingles Squared");
		case Ingles3EquationType::I_Cubed:
			return _T("Ingles Cubed");
		case Ingles3EquationType::I_SinX:
			return _T("Ingles Sine X");
		case Ingles3EquationType::I_Alt1:
			return _T("Ingles Alt 1");
		case Ingles3EquationType::I_Alt2:
			return _T("Ingles Alt 2");
		default:
			assert(false);
			return _T("Unknown");
		}
	}
}