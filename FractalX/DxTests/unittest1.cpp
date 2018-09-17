#include "stdafx.h"
#include "CppUnitTest.h"
#include <vector>
#include "DxfColorFactory.h"
#include <CppUnitTestAssert.h>
#include "RotationParams.h"
#include "SeedTriangles.h"
#include "SphereApproximator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DXF;

namespace DxTests
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestRainbow)
		{
			std::vector<uint32_t> colors = DXF::CreateRainbow();
			Assert::AreEqual(static_cast<int>(colors.size()), 9, L"CreateRainbow did not produce 9 colors", LINE_INFO());
		}

		TEST_METHOD(TestActionFromInt)
		{
			RotationAction a1 = RotationActionFromInt(1);
			Assert::IsTrue(a1 == RotationAction::RotateAll, L"Rotate all from 1 failed", LINE_INFO());

			RotationAction a0 = RotationActionFromInt(0);
			Assert::IsTrue(a0 == RotationAction::Fixed, L"Fixed from 0 failed", LINE_INFO());

			RotationAction a5 = RotationActionFromInt(5);
			Assert::IsTrue(a5 == RotationAction::RotateXY, L"Rotate XY from 5 failed", LINE_INFO());

			RotationAction a7 = RotationActionFromInt(7);
			Assert::IsTrue(a7 == RotationAction::RotateXZ, L"Rotate XZ from 7 failed", LINE_INFO());
		}

		TEST_METHOD(TestActionToInt)
		{
			int a0 = RotationActionToInt(RotationAction::Fixed);
			Assert::AreEqual(a0, 0, L"Fixed to 0 failed", LINE_INFO());

			int a2 = RotationActionToInt(RotationAction::RotateX);
			Assert::AreEqual(a2, 2, L"Rotate X to 2 failed", LINE_INFO());

			int a4 = RotationActionToInt(RotationAction::RotateZ);
			Assert::AreEqual(a4, 4, L"Rotate Z to 4 failed", LINE_INFO());

			int a7 = RotationActionToInt(RotationAction::RotateXZ);
			Assert::AreEqual(a7, 7, L"Rotate XZ to 7 failed", LINE_INFO());
		}

		TEST_METHOD(TestGenerateCrudeTriangles1)
		{
			auto prog = [](double) { };
			auto data0 = GenerateCrudeTriangles(0, SeedTriangles::Eight, prog);
			Assert::AreEqual(6, static_cast<int>(data0->Vertices.size()), L"zero iterations should have 6 vertices", LINE_INFO());
			Assert::AreEqual(8, static_cast<int>(data0->Triangles.size()), L"zero iterations should have 8 triangles", LINE_INFO());

			auto data1 = GenerateCrudeTriangles(1, SeedTriangles::Eight, prog);
			Assert::AreEqual(18, static_cast<int>(data1->Vertices.size()), L"one iterations should have 28 vertices", LINE_INFO());
			Assert::AreEqual(32, static_cast<int>(data1->Triangles.size()), L"one iterations should have 32 triangles", LINE_INFO());

			auto data2 = GenerateCrudeTriangles(2, SeedTriangles::Eight, prog);
			Assert::AreEqual(66, static_cast<int>(data2->Vertices.size()), L"two iterations should have 66 vertices", LINE_INFO());
			Assert::AreEqual(128, static_cast<int>(data2->Triangles.size()), L"two iterations should have 128 triangles", LINE_INFO());

			auto data3 = GenerateCrudeTriangles(3, SeedTriangles::Eight, prog);
			Assert::AreEqual(258, static_cast<int>(data3->Vertices.size()), L"three iterations should have 258 vertices", LINE_INFO());
			Assert::AreEqual(512, static_cast<int>(data3->Triangles.size()), L"three iterations should have 512 triangles", LINE_INFO());

			auto data5 = GenerateCrudeTriangles(5, SeedTriangles::Eight, prog);
			Assert::AreEqual(4098, static_cast<int>(data5->Vertices.size()), L"five iterations should have 4098 vertices", LINE_INFO());
			Assert::AreEqual(8192, static_cast<int>(data5->Triangles.size()), L"five iterations should have 8192  triangles", LINE_INFO());
		}
/*
		TEST_METHOD(TestFailure)
		{
			Assert::Fail(L"Just testing");
		}
*/
	};
}