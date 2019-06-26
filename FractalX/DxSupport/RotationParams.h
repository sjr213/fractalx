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

	enum class RotationSpeed
	{
		Slow,
		Medium,
		Fast
	};

	RotationAction RotationActionFromInt(int val);

	int RotationActionToInt(RotationAction action);

	RotationSpeed RotationSpeedFromInt(int val);

	int RotationSpeedToInt(RotationSpeed speed);

	int GetRotationSpeedMsec(RotationSpeed speed);

	struct RotationParams
	{
		static const int RotationVersion = 1;

		RotationParams()
			: Action(RotationAction::Fixed)
			, AngleXDegrees(0)
			, AngleYDegrees(0)
			, AngleZDegrees(0)
			, Speed(RotationSpeed::Medium)
		{}

		RotationParams(RotationAction action, float angleX, float angleY, float angleZ)
			: Action(action)
			, AngleXDegrees(angleX)
			, AngleYDegrees(angleY)
			, AngleZDegrees(angleZ)
			, Speed(RotationSpeed::Medium)
		{}

		RotationAction Action;
		float AngleXDegrees;
		float AngleYDegrees;
		float AngleZDegrees;
		RotationSpeed Speed;
	};
}
