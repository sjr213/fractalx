
#include "stdafx.h"
#include "RotationParams.h"

namespace DXF
{
	RotationAction RotationActionFromInt(int val)
	{
		switch (val)
		{
		case 0:
			return RotationAction::Fixed;
		case 1:
			return RotationAction::RotateAll;
		case 2:
			return RotationAction::RotateX;
		case 3:
			return RotationAction::RotateY;
		case 4:
			return RotationAction::RotateZ;
		case 5:
			return RotationAction::RotateXY;
		case 6:
			return RotationAction::RotateYZ;
		case 7:
			return RotationAction::RotateXZ;
		default:
			return RotationAction::RotateAll;
		}
	}

	int RotationActionToInt(RotationAction action)
	{
		switch (action)
		{
		case RotationAction::Fixed:
			return 0;
		case RotationAction::RotateAll:
			return 1;
		case RotationAction::RotateX:
			return 2;
		case RotationAction::RotateY:
			return 3;
		case RotationAction::RotateZ:
			return 4;
		case RotationAction::RotateXY:
			return 5;
		case RotationAction::RotateYZ:
			return 6;
		case RotationAction::RotateXZ:
			return 7;
		default:
			return 1;
		}
	}

	RotationSpeed RotationSpeedFromInt(int val)
	{
		switch (val)
		{
		case 0:
			return RotationSpeed::Slow;
		case 1:
			return RotationSpeed::Medium;
		case 2:
			return RotationSpeed::Fast;

		default:
			return RotationSpeed::Medium;
		}
	}

	int RotationSpeedToInt(RotationSpeed speed)
	{
		switch (speed)
		{
		case RotationSpeed::Slow:
			return 0;
		case RotationSpeed::Medium:
			return 1;
		case RotationSpeed::Fast:
			return 2;
		default:
			return 1;
		}
	}

	int GetRotationSpeedMsec(RotationSpeed speed)
	{
		switch (speed)
		{
		case RotationSpeed::Slow:
			return 500;
		case RotationSpeed::Medium:
			return 200;
		case RotationSpeed::Fast:
			return 50;
		default:
			return 200;
		}
	}
}