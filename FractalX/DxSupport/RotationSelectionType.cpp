#include "stdafx.h"
#include "RotationSelectionType.h"

#include <assert.h>

namespace DXF
{
	int RotationSelectionTypeToInt(RotationSelectionType rotationType)
	{
		switch (rotationType)
		{
		case RotationSelectionType::MainModel:
			return 0;
		case RotationSelectionType::Background:
			return 1;
		case RotationSelectionType::LockBackgroundOnModel:
			return 2;
		default:
			assert(false);
			return 0;
		}
	}

	RotationSelectionType RotationSelectionTypeFromInt(int type)
	{
		switch (type)
		{
		case 0:
			return RotationSelectionType::MainModel;
		case 1:
			return RotationSelectionType::Background;
		case 2:
			return RotationSelectionType::LockBackgroundOnModel;
		default:
			assert(false);
			return RotationSelectionType::MainModel;
		}
	}
}