#pragma once

#include "TrigOption.h"

namespace DXF
{
	struct CartesianConverterGroup
	{
		double MultiplerX;
		TrigOption ThetaOptionX;
		TrigOption PhiOptionX;
		double MultiplerY;
		TrigOption ThetaOptionY;
		TrigOption PhiOptionY;
		double MultiplerZ;
		TrigOption ThetaOptionZ;
		TrigOption PhiOptionZ;

		CartesianConverterGroup();
		CartesianConverterGroup(const CartesianConverterGroup&);
		CartesianConverterGroup& operator=(const CartesianConverterGroup& rhs);
		bool operator==(const CartesianConverterGroup& rhs) const;
		bool operator!=(const CartesianConverterGroup& rhs) const;

		virtual ~CartesianConverterGroup();
	};
}