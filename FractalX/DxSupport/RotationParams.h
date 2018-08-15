#pragma once

namespace DXF
{
	enum class RotationAction
	{
		Fixed,
		RotateAll,
		RotateX,
		RotateY,
		RotateZ,
		RotateXY,
		RotateYZ,
		RotateXZ
	};

	RotationAction RotationActionFromInt(int val);

	int RotationActionToInt(RotationAction action);

	struct RotationParams
	{
		RotationParams()
			: Action(RotationAction::Fixed)
			, AngleXDegrees(0)
			, AngleYDegrees(0)
			, AngleZDegrees(0)
		{}

		RotationParams(RotationAction action, float angleX, float angleY, float angleZ)
			: Action(action)
			, AngleXDegrees(angleX)
			, AngleYDegrees(angleY)
			, AngleZDegrees(angleZ)
		{}

		RotationAction Action;
		float AngleXDegrees;
		float AngleYDegrees;
		float AngleZDegrees;
	};
}
