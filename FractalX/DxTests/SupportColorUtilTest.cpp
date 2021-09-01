#include "stdafx.h"
#include "CppUnitTest.h"
#include <CppUnitTestAssert.h>
#include "ColorUtil.h"
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DxTests
{
	TEST_CLASS(supportColorUtilTest)
	{
	public:
		TEST_METHOD(TestMakeColor)
		{
			auto color = DXF::ColorUtil::MakeColor(1, 1, 1, 1);
			uint32_t expectedColor = 16843009;

			Assert::AreEqual(expectedColor, color, L"1,1,1,1 not equal 16843009", LINE_INFO());
		}

		TEST_METHOD(TestMakeColor2)
		{
			auto color = DXF::ColorUtil::MakeColor(0, 0, 0, 125);
			uint32_t expectedColor = 125;

			Assert::AreEqual(expectedColor, color, L"0,0,0,125 not equal 125", LINE_INFO());
		}

		TEST_METHOD(TestMakeColor3)
		{
			auto color = DXF::ColorUtil::MakeColor(100, 0, 0, 0);
			uint32_t expectedColor = 1677721600;

			Assert::AreEqual(expectedColor, color, L"100,0,0,0 not equal 1677721600", LINE_INFO());
		}

		TEST_METHOD(TestGetComponent)
		{
			auto color = DXF::ColorUtil::MakeColor(5, 40, 100, 156);

			auto b1 = DXF::ColorUtil::GetComponent(color, 0);
			Assert::AreEqual((byte)5, b1, L"5 should equal first component");

			auto b2 = DXF::ColorUtil::GetComponent(color, 1);
			Assert::AreEqual((byte)40, b2, L"40 should equal 2nd component");

			auto b3 = DXF::ColorUtil::GetComponent(color, 2);
			Assert::AreEqual((byte)100, b3, L"100 should equal 3rd component");

			auto b4 = DXF::ColorUtil::GetComponent(color, 3);
			Assert::AreEqual((byte)156, b4, L"156 should equal first component");
		}

		TEST_METHOD(TestArgbToRgba)
		{
			auto color = DXF::ColorUtil::MakeColor(5, 40, 100, 156);
			auto rgba = DXF::ColorUtil::ArgbToRgba(color);

			auto b1 = DXF::ColorUtil::GetComponent(rgba, 0);
			Assert::AreEqual((byte)40, b1, L"40 should equal first component");

			auto b2 = DXF::ColorUtil::GetComponent(rgba, 1);
			Assert::AreEqual((byte)100, b2, L"100 should equal 2nd component");

			auto b3 = DXF::ColorUtil::GetComponent(rgba, 2);
			Assert::AreEqual((byte)156, b3, L"156 should equal 3rd component");

			auto b4 = DXF::ColorUtil::GetComponent(rgba, 3);
			Assert::AreEqual((byte)5, b4, L"5 should equal first component");
		}

		TEST_METHOD(ConvertFromARGBtoRGBA)
		{
			auto color1 = DXF::ColorUtil::MakeColor(5, 40, 100, 156);
			auto color2 = DXF::ColorUtil::MakeColor(1, 1, 1, 1);

			std::vector<uint32_t> v;
			v.push_back(color1);
			v.push_back(color2);

			auto result = DXF::ColorUtil::ConvertFromARGBtoRGBA(v);
			int size = static_cast<int>(result.size());
			Assert::AreEqual(2, size, L"Expected 2 elements in vector");

			auto c1 = result.at(0);
			auto b1 = DXF::ColorUtil::GetComponent(c1, 0);
			Assert::AreEqual((byte)40, b1, L"40 should equal first component");

			auto b2 = DXF::ColorUtil::GetComponent(c1, 1);
			Assert::AreEqual((byte)100, b2, L"100 should equal 2nd component");

			auto b3 = DXF::ColorUtil::GetComponent(c1, 2);
			Assert::AreEqual((byte)156, b3, L"156 should equal 3rd component");

			auto b4 = DXF::ColorUtil::GetComponent(c1, 3);
			Assert::AreEqual((byte)5, b4, L"5 should equal first component");

			auto c2 = result.at(1);
			uint32_t expectedColor = 16843009;

			Assert::AreEqual(expectedColor, c2, L"1,1,1,1 not equal 16843009", LINE_INFO());
		}

	};
}