#pragma once

namespace DXF
{
	enum class RotationSelectionType
	{
		MainModel = 0,
		Background = 1,
		LockBackgroundOnModel = 2
	};

	int RotationSelectionTypeToInt(RotationSelectionType rotationType);

	RotationSelectionType RotationSelectionTypeFromInt(int type);
}