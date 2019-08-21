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
		const int FractalParamVersion = 3;

		if (ar.IsStoring())
		{
			ar << FractalParamVersion;
			ar << fractalParams.ConstantC << fractalParams.Power << fractalParams.Bailout;
			ar << FractalTypeToInt(fractalParams.FractalModelType);
			ar << CartesianConversionTypeToInt(fractalParams.CartesianType);
			ar << BulbNormalizeTypeToInt(fractalParams.NormalizationType);
			ar << fractalParams.NormalizationRoot;
		}
		else
		{
			int version = 0;
			ar >> version;

			if (version < 1 || version > FractalParamVersion)
			{
				assert(false);
				return;
			}
				
			ar >> fractalParams.ConstantC >> fractalParams.Power >> fractalParams.Bailout;
	
			if (version < 2)
				return;

			int type = 1;
			ar >> type;

			fractalParams.FractalModelType = FractalTypeFromInt(type);

			if (version < 3)
				return;

			int cartesianType = 1;
			ar >> cartesianType;
			fractalParams.CartesianType = CartesianConversionTypeFromInt(cartesianType);

			int normalizationType = 1;
			ar >> normalizationType;
			fractalParams.NormalizationType = BulbNormalizeTypeFromInt(normalizationType);

			ar >> fractalParams.NormalizationRoot;
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