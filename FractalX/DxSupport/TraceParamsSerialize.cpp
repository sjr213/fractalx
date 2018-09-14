#include "stdafx.h"
#include "TraceParamsSerialize.h"
#include "TraceParams.h"
#include "DxSerialize.h"

namespace DXF
{
	void Serialize(CArchive& ar, StretchDistanceParams& stretchParams)
	{
		const int StretchVersion = 1;

		if (ar.IsStoring())
		{
			ar << StretchVersion;
			ar << stretchParams.StretchDistance << stretchParams.EstimateMinMax << 
				stretchParams.MinDistance << stretchParams.MaxDistance;
		}
		else
		{
			int version = 0;
			ar >> version;

			assert(version == StretchVersion);
			if (version != StretchVersion)
				return;

			ar >> stretchParams.StretchDistance >> stretchParams.EstimateMinMax >>
				stretchParams.MinDistance >> stretchParams.MaxDistance;
		}
	}

	void Serialize(CArchive& ar, FractalParams& fractalParams)
	{
		const int FractalParamVersion = 1;

		if (ar.IsStoring())
		{
			ar << FractalParamVersion;
			ar << fractalParams.ConstantC << fractalParams.Power << fractalParams.Bailout;
		}
		else
		{
			int version = 0;
			ar >> version;

			assert(version == FractalParamVersion);
			if (version != FractalParamVersion)
				return;

			ar >> fractalParams.ConstantC >> fractalParams.Power >> fractalParams.Bailout;
		}
	}

	void Serialize(CArchive& ar, BulbParams& bulbParams)
	{
		const int BulbParamVersion = 1;

		if (ar.IsStoring())
		{
			ar << BulbParamVersion;
			Serialize(ar, bulbParams.Origin);

			ar << bulbParams.Distance << bulbParams.MaxRaySteps << bulbParams.MinRayDistance;
			ar << bulbParams.StepDivisor << bulbParams.Iterations << bulbParams.NormalDelta << bulbParams.Fractional;
		}
		else
		{
			int version = 0;
			ar >> version;

			assert(version == BulbParamVersion);
			if (version != BulbParamVersion)
				return;

			Serialize(ar, bulbParams.Origin);

			ar >> bulbParams.Distance >> bulbParams.MaxRaySteps >> bulbParams.MinRayDistance;
			ar >> bulbParams.StepDivisor >> bulbParams.Iterations >> bulbParams.NormalDelta >> bulbParams.Fractional;
		}
	}

	void Serialize(CArchive& ar, TraceParams& traceParams)
	{
		const int TraceParamVersion = 1;

		if (ar.IsStoring())
		{
			ar << TraceParamVersion;
		}
		else
		{
			int version = 0;
			ar >> version;

			assert(version == TraceParamVersion);
			if (version != TraceParamVersion)
				return;
		}

		Serialize(ar, traceParams.Fractal);
		Serialize(ar, traceParams.Bulb);
		Serialize(ar, traceParams.Stretch);
	}

}