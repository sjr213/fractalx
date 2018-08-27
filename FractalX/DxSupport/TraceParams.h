#pragma once
#include <SimpleMath.h>

namespace DXF
{
	struct StretchDistanceParams
	{
		bool StretchDistance = false;
		bool EstimateMinMax = false;
		double MinDistance = 0.0;
		double MaxDistance = 0.03;
	};

	struct TraceParams
	{
		double ConstantC = 1.0;
		double Power = 8.0;
		float Distance = 10.0f;
		DirectX::SimpleMath::Vector3 Origin = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);

		// Distance estimate params
		int MaxRaySteps = 1000;
		double MinRayDistance = 0.0001;
		double StepDivisor = 10.0;
		double Bailout = 2.0;
		int Iterations = 256;
		float NormalDelta = 0.01f;

		bool Fractional = false;

		StretchDistanceParams StretchParams;
	};

}