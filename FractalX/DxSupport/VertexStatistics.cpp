
#include "stdafx.h"
#include "VertexStatistics.h"

namespace DXF
{
	namespace VertexStatistics
	{
		static int GetLevels(int iterations)
		{
			int levels = 2;
			for (int i = 1; i <= iterations; ++i)
				levels += levels - 1;

			return levels;
		}

		static int GetVerticesPerSingleTriangleLevels(int levels)
		{
			int v = 0;
			for (int k = levels; k > 0; --k)
				v += k;

			return v;
		}

		static int CalculateNumberOfVerticesForOneTriangle(int iterations)
		{
			if (iterations == 0)
				return 3;

			int levels = GetLevels(iterations);

			int v = GetVerticesPerSingleTriangleLevels(levels);

			return v;
		}

		static int CalculateNumberOfVerticesForTwoTriangles(int iterations)
		{
			if (iterations == 0)
				return 4;

			int levels = GetLevels(iterations);

			int v = GetVerticesPerSingleTriangleLevels(levels);

			return 2 * v - levels;
		}

		static int CalculateNumberOfVerticesForFourTriangles(int iterations)
		{
			if (iterations == 0)
				return 6;

			int levels = GetLevels(iterations);

			int v = GetVerticesPerSingleTriangleLevels(levels);

			return (4 * v) - (3 * levels);
		}

		static int CalculateNumberOfVerticesForEightTriangles(int iterations)
		{
			double exp = iterations + 1;

			double nV = 2.0 + pow(4.0, exp);

			return static_cast<int>(nV + 0.5);
		}

		static int CalculateNumberOfTrianglesForOneTriangle(int iterations)
		{
			double exp = iterations;

			double nV = pow(4.0, exp);

			return static_cast<int>(nV + 0.5);
		}

		static int CalculateNumberOfTrianglesForTwoTriangles(int iterations)
		{
			double exp = iterations;

			double nV = 2.0 * pow(4.0, exp);

			return static_cast<int>(nV + 0.5);
		}

		static int CalculateNumberOfTrianglesForFourTriangles(int iterations)
		{
			double exp = iterations + 1;

			double nT = pow(4.0, exp);

			return static_cast<int>(nT + 0.5);
		}

		static int CalculateNumberOfTrianglesForEightTriangles(int iterations)
		{
			double exp = iterations + 1;

			double nT = 2.0 * pow(4.0, exp);

			return static_cast<int>(nT + 0.5);
		}

		int CalculateNumberOfVertices(int iterations, SeedTriangles seedTriangles)
		{
			if (iterations < 0)
				return 0;

			if (seedTriangles == SeedTriangles::One)
				return CalculateNumberOfVerticesForOneTriangle(iterations);

			if (seedTriangles == SeedTriangles::Two)
				return CalculateNumberOfVerticesForTwoTriangles(iterations);

			if (seedTriangles == SeedTriangles::Four)
				return CalculateNumberOfVerticesForFourTriangles(iterations);

			if (seedTriangles == SeedTriangles::Eight)
				return CalculateNumberOfVerticesForEightTriangles(iterations);

			return 0;
		}

		int CalculateNumberOfTriangles(int iterations, SeedTriangles seedTriangles)
		{
			if (iterations < 0)
				return 0;

			if (seedTriangles == SeedTriangles::One)
				return CalculateNumberOfTrianglesForOneTriangle(iterations);

			if (seedTriangles == SeedTriangles::Two)
				return CalculateNumberOfTrianglesForTwoTriangles(iterations);

			if (seedTriangles == SeedTriangles::Four)
				return CalculateNumberOfTrianglesForFourTriangles(iterations);

			if (seedTriangles == SeedTriangles::Eight)
				return CalculateNumberOfTrianglesForEightTriangles(iterations);

			return 0;
		}
	}
}