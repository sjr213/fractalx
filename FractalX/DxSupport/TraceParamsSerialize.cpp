#include "stdafx.h"
#include "TraceParamsSerialize.h"

#include "TraceParams.h"
#include "DxSerialize.h"
#include "VertexSerialization.h"

namespace DXF
{
	void Serialize(CArchive& ar, CartesianConverterGroup& gr)
	{
		const int CCGVersion = 1;

		if (ar.IsStoring())
		{
			ar << CCGVersion;
			ar << gr.MultiplerX << TrigOptionToInt(gr.ThetaOptionX) <<TrigOptionToInt(gr.PhiOptionX);
			ar << gr.MultiplerY << TrigOptionToInt(gr.ThetaOptionY) << TrigOptionToInt(gr.PhiOptionY);
			ar << gr.MultiplerZ << TrigOptionToInt(gr.ThetaOptionZ) << TrigOptionToInt(gr.PhiOptionZ);
		}
		else
		{
			int version = 0;
			ar >> version;

			assert(version == CCGVersion);
			if (version != CCGVersion)
				return;

			int thetaOpt = 0, phiOpt = 0;

			ar >> gr.MultiplerX >> thetaOpt >> phiOpt;
			gr.ThetaOptionX = TrigOptionFromInt(thetaOpt);
			gr.PhiOptionX = TrigOptionFromInt(phiOpt);

			ar >> gr.MultiplerY >> thetaOpt >> phiOpt;
			gr.ThetaOptionY = TrigOptionFromInt(thetaOpt);
			gr.PhiOptionY = TrigOptionFromInt(phiOpt);

			ar >> gr.MultiplerZ >> thetaOpt >> phiOpt;
			gr.ThetaOptionZ = TrigOptionFromInt(thetaOpt);
			gr.PhiOptionZ = TrigOptionFromInt(phiOpt);
		}
	}

	void Serialize(CArchive& ar, StretchDistanceParams& stretchParams)
	{
		const int StretchVersion = 2;

		if (ar.IsStoring())
		{
			ar << StretchVersion;
			ar << stretchParams.StretchDistance << stretchParams.EstimateMinMax <<
				stretchParams.MinDistance << stretchParams.MaxDistance;
			ar << stretchParams.Stretch;
		}
		else
		{
			int version = 0;
			ar >> version;

			assert(version >= 1);
			if (version < 1)
				return;

			ar >> stretchParams.StretchDistance >> stretchParams.EstimateMinMax >>
				stretchParams.MinDistance >> stretchParams.MaxDistance;

			if (version > 1)
				ar >> stretchParams.Stretch;
		}
	}

	void Serialize(CArchive& ar, FractalParams& fractalParams)
	{
		const int FractalParamVersion = 6;

		if (ar.IsStoring())
		{
			ar << FractalParamVersion;
			ar << fractalParams.Derivative << fractalParams.Power << fractalParams.Bailout;
			ar << FractalTypeToInt(fractalParams.FractalModelType);
			ar << CartesianConversionTypeToInt(fractalParams.CartesianType);
			ar << BulbNormalizeTypeToInt(fractalParams.NormalizationType);
			ar << fractalParams.NormalizationRoot;
			Serialize(ar, fractalParams.ConversionGroup);
			SerializeVertex(ar, fractalParams.ConstantC);
			ar << Ingles3EquationTypeToInt(fractalParams.InglesEquation);
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
				
			ar >> fractalParams.Derivative >> fractalParams.Power >> fractalParams.Bailout;
	
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

			if (version < 4) 
				return;

			Serialize(ar, fractalParams.ConversionGroup);

			if (version < 5) 
				return;

			SerializeVertex(ar, fractalParams.ConstantC);

			if (version < 6) 
				return;

			int inglesType = 1;
			ar >> inglesType;
			fractalParams.InglesEquation = Ingles3EquationTypeFromInt(inglesType);
		}
	}

	void Serialize(CArchive& ar, BulbParams& bulbParams)
	{
		const int BulbParamVersion = 1;

		float normalDeltaObsolete = 0.0;

		if (ar.IsStoring())
		{
			ar << BulbParamVersion;
			Serialize(ar, bulbParams.Origin);

			ar << bulbParams.Distance << bulbParams.MaxRaySteps << bulbParams.MinRayDistance;
			ar << bulbParams.StepDivisor << bulbParams.Iterations << normalDeltaObsolete << bulbParams.Fractional;
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
			ar >> bulbParams.StepDivisor >> bulbParams.Iterations >> normalDeltaObsolete >> bulbParams.Fractional;
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