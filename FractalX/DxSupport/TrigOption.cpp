#include "stdafx.h"
#include "TrigOption.h"

#include <assert.h>

namespace DXF
{
	CString TrigOptionString(TrigOption opt)
	{
		switch (opt)
		{
		case TrigOption::None:
			return _T("None");
		case TrigOption::Sin:
			return _T("Sin");
		case TrigOption::Cos:
			return _T("Cos");
		case TrigOption::Tan:
			return _T("Tan");
		default:
			assert(false);
			return _T("Unknown");
		}
	}

	int TrigOptionToInt(TrigOption opt)
	{
		switch (opt)
		{
		case TrigOption::None:
			return 0;
		case TrigOption::Sin:
			return 1;
		case TrigOption::Cos:
			return 2;
		case TrigOption::Tan:
			return 3;
		default:
			assert(false);
			return 0;
		}
	}

	TrigOption TrigOptionFromInt(int i)
	{
		switch (i)
		{
		case(0):
			return TrigOption::None;
		case(1):
			return TrigOption::Sin;
		case(2):
			return TrigOption::Cos;
		case(3):
			return TrigOption::Tan;
		default:
			assert(false);
			return TrigOption::None;
		}
	}
}