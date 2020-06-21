#pragma once

#include "RotationParams.h"
#include "RotationSelectionType.h"

namespace DXF
{
	struct RotationGroup
	{
		static const int RotationGroupVersion = 1;

		RotationSelectionType RotationType;
		RotationParams RotationParamsMain;
		RotationParams RotationParamsBackground;		
	};
}