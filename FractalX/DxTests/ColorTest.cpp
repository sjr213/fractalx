#include "stdafx.h"
#include "CppUnitTest.h"
#include <CppUnitTestAssert.h>
#include "hslUtilities.h"
#include "ColorArgb.h"
#include "ColorContrast.h"
#include <iosfwd>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DxColor;

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {

			template<>
			static std::wstring ToString<DxColor::ColorArgb>(const DxColor::ColorArgb  & argb) {
				std::wstringstream str;
				str << L"a r g b:";
				str << argb.A << L", ";
				str << argb.R << L", ";
				str << argb.G << L", ";
				str << argb.B << L'.';
				return str.str();
			}

		}
	}
}

namespace DxTests
{
	TEST_CLASS(colorTest)
	{
	public:
		TEST_METHOD(TestBlack)
		{
			ColorArgb black(255, 0, 0, 0);
			HSL hsl = ToHsl(black);

			auto retVal = ToRgb(hsl, 255);

			Assert::AreEqual(black, retVal, L"Black not converted back", LINE_INFO());
		}

		TEST_METHOD(TestRed)
		{
			ColorArgb red(255, 255, 0, 0);
			HSL hsl = ToHsl(red);

			auto retVal = ToRgb(hsl, 255);

			Assert::AreEqual(red, retVal, L"Red not converted back", LINE_INFO());
		}

		TEST_METHOD(TestGreen)
		{
			ColorArgb green(255, 0, 255, 0);
			HSL hsl = ToHsl(green);

			auto retVal = ToRgb(hsl, 255);

			Assert::AreEqual(green, retVal, L"Green not converted back", LINE_INFO());
		}

		TEST_METHOD(TestBlue)
		{
			ColorArgb blue(255, 0, 0, 255);
			HSL hsl = ToHsl(blue);

			auto retVal = ToRgb(hsl, 255);

			Assert::AreEqual(blue, retVal, L"Blue not converted back", LINE_INFO());
		}

		TEST_METHOD(TestMagenta)
		{
			ColorArgb magenta(255, 255, 0, 255);
			HSL hsl = ToHsl(magenta);

			auto retVal = ToRgb(hsl, 255);

			Assert::AreEqual(magenta, retVal, L"Magenta not converted back", LINE_INFO());
		}

		TEST_METHOD(TestWhite)
		{
			ColorArgb white(255, 255, 255, 255);
			HSL hsl = ToHsl(white);

			auto retVal = ToRgb(hsl, 255);

			Assert::AreEqual(white, retVal, L"White not converted back", LINE_INFO());
		}

  		TEST_METHOD(TestArbitrary)
		{
			ColorArgb arb(255, 120, 100, 37);
			HSL hsl = ToHsl(arb);

			auto retVal = ToRgb(hsl, 255);

			Assert::AreEqual(arb, retVal, L"Arbitrary color not converted back", LINE_INFO());
		}
	};
}