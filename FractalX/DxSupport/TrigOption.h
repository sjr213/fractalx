#pragma once

namespace DXF
{
	enum class TrigOption
	{
		None = 0,
		Sin = 1,
		Cos = 2,
		Tan = 3
	};

	CString TrigOptionString(TrigOption opt);
	int TrigOptionToInt(TrigOption opt);
	TrigOption TrigOptionFromInt(int i);
}