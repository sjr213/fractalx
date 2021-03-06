#pragma once

#include <SimpleMath.h>

#include "BulbNormalizeType.h"
#include "CartesianConverterGroup.h"
#include "CartesianConversionType.h"
#include "DefaultFields.h"
#include "FractalType.h"
#include "Ingles3EquantionType.h"
#include "Vertex.h"


namespace DXF
{
	struct StretchDistanceParams
	{
		bool StretchDistance = false;
		bool EstimateMinMax = false;
		double MinDistance = 0.0;
		double MaxDistance = 0.03;
		bool Stretch = false;
	};

	struct FractalParams
	{
		double Derivative = 1.0;
		double Power = 8.0;		
		double Bailout = 2.0;
		FractalType FractalModelType = FractalType::StandardBulb;
		CartesianConversionType CartesianType = CartesianConversionType::StandardConversion;
		BulbNormalizeType NormalizationType = BulbNormalizeType::StandardNormalization;
		double NormalizationRoot = 0.5;
		CartesianConverterGroup ConversionGroup;
		Vertex<double> ConstantC = Vertex<double>(0.0,0.0,0.0);
		Ingles3EquationType InglesEquation = Ingles3EquationType::I_Squared;
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

		bool Fractional = false;
	};

	struct BackgroundImageParams
	{
		std::wstring ImageFilename;
		bool ShowBackgroundModel = false;
		std::vector<Vertex<float>> BackgroundModel = GetDefaultBackgroundModel();
	};

	struct TraceParams
	{
		FractalParams Fractal;

		BulbParams Bulb;

		StretchDistanceParams Stretch;

		BackgroundImageParams Background;
	};

}