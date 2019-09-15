#include "stdafx.h"
#include "CartesianConverterGroup.h"

namespace DXF
{
	CartesianConverterGroup::CartesianConverterGroup()
	{
		MultiplerX = 1;
		ThetaOptionX = TrigOption::Sin;
		PhiOptionX = TrigOption::Cos;
		MultiplerY = 1;
		ThetaOptionY = TrigOption::Sin;
		PhiOptionY = TrigOption::Sin;
		MultiplerZ = 1;
		ThetaOptionZ = TrigOption::Cos;
		PhiOptionZ = TrigOption::None;
	}

	CartesianConverterGroup::CartesianConverterGroup(const CartesianConverterGroup& g)
		: MultiplerX(g.MultiplerX)
		, ThetaOptionX(g.ThetaOptionX)
		, PhiOptionX(g.PhiOptionX)
		, MultiplerY(g.MultiplerY)
		, ThetaOptionY(g.ThetaOptionY)
		, PhiOptionY(g.PhiOptionY)
		, MultiplerZ(g.MultiplerZ)
		, ThetaOptionZ(g.ThetaOptionZ)
		, PhiOptionZ(g.PhiOptionZ)
	{}

	CartesianConverterGroup::~CartesianConverterGroup()
	{}

	CartesianConverterGroup& CartesianConverterGroup::operator=(const CartesianConverterGroup& rhs)
	{
		if (*this == rhs)
			return *this;

		MultiplerX = rhs.MultiplerX;
		ThetaOptionX = rhs.ThetaOptionX;
		PhiOptionX = rhs.PhiOptionX;
		MultiplerY = rhs.MultiplerY;
		ThetaOptionY = rhs.ThetaOptionY;
		PhiOptionY = rhs.PhiOptionY;
		MultiplerZ = rhs.MultiplerZ;
		ThetaOptionZ = rhs.ThetaOptionZ;
		PhiOptionZ = rhs.PhiOptionZ;

		return *this;
	}

	bool CartesianConverterGroup::operator==(const CartesianConverterGroup& rhs) const
	{
		return MultiplerX == rhs.MultiplerX &&
			ThetaOptionX == rhs.ThetaOptionX &&
			PhiOptionX == rhs.PhiOptionX &&
			MultiplerY == rhs.MultiplerY &&
			ThetaOptionY == rhs.ThetaOptionY &&
			PhiOptionY == rhs.PhiOptionY &&
			MultiplerZ == rhs.MultiplerZ &&
			ThetaOptionZ == rhs.ThetaOptionZ &&
			PhiOptionZ == rhs.PhiOptionZ;
	}

	bool CartesianConverterGroup::operator!=(const CartesianConverterGroup& rhs) const
	{
		return !(*this == rhs);
	}
}