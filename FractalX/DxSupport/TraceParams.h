#pragma once
#include <SimpleMath.h>

namespace DXF
{
	enum class FractalType
	{
		StandardBulb = 1,
		CartesianConvertAltX1 = 2
	};

	int FractalTypeToInt(FractalType fractalType);

	FractalType FractalTypeFromInt(int type);

	CString FractalTypeString(FractalType fractalType);

	struct StretchDistanceParams
	{
		bool StretchDistance = false;
		bool EstimateMinMax = false;
		double MinDistance = 0.0;
		double MaxDistance = 0.03;
	};

	struct FractalParams
	{
		double ConstantC = 1.0;
		double Power = 8.0;		
		double Bailout = 2.0;
		FractalType FractalModelType = FractalType::StandardBulb;
	};

	struct BulbParams
	{
		DirectX::XMFLOAT3 Origin = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		float Distance = 10.0f;

		// Distance estimate params
		int MaxRaySteps = 1000;
		double MinRayDistance = 0.0001;
		double StepDivisor = 10.0;

		int Iterations = 256;
		float NormalDelta = 0.01f;

		bool Fractional = false;
	};

	struct TraceParams
	{
		FractalParams Fractal;

		BulbParams Bulb;

		StretchDistanceParams Stretch;
	};

}