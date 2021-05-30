#include "stdafx.h"
#include "CppUnitTest.h"
#include <CppUnitTestAssert.h>

#include "NarrowCast.h"

using namespace DxCore;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DxTests
{
	TEST_CLASS(narrowCastTest)
	{
	public:
		TEST_METHOD(TestIntToBite)
		{
			try
			{
				int x = 10;
				unsigned __int8 result = narrow_cast<unsigned __int8>(x);
				Assert::AreEqual(result, 10ui8, L"Narrow cast 10 to bite failed", LINE_INFO());
			}
			catch (bad_narrow_cast&)
			{
				Assert::Fail(L"Narrow cast 10 to bite exception", LINE_INFO());
			}
		}

		TEST_METHOD(TestIntToBiteShouldFail)
		{
			try
			{
				int x = 300;
				unsigned __int8 result = narrow_cast<unsigned __int8>(x);
				Assert::Fail(L"Narrow cast 300 to bite should have failed", LINE_INFO());
			}
			catch (bad_narrow_cast&)
			{
			}
		}

		TEST_METHOD(TestIntToUInt)
		{
			try
			{
				int x = 500;
				unsigned int result = narrow_cast<unsigned int>(x);
				Assert::AreEqual(result, 500u, L"Narrow cast 500 to uint failed", LINE_INFO());
			}
			catch (bad_narrow_cast&)
			{
				Assert::Fail(L"Narrow cast 500 to uint exception", LINE_INFO());
			}
		}

		TEST_METHOD(TestNegativeIntToUInt)
		{
			try
			{
				int x = -500;
				unsigned int result = narrow_cast<unsigned int>(x);
				Assert::Fail(L"Narrow cast -500 to unit should have failed", LINE_INFO());
			}
			catch (bad_narrow_cast&)
			{
			}
		}

		TEST_METHOD(TestDoubleToFloat)
		{
			try
			{
				double x = 3.14;
				float result = narrow_cast<float>(x);
				Assert::AreEqual(result, 3.14f, 0.00001f, L"Narrow cast 500 to uint failed", LINE_INFO());
			}
			catch (bad_narrow_cast&)
			{
				Assert::Fail(L"Narrow cast 3.14 to float threw exception", LINE_INFO());
			}
		}

		TEST_METHOD(TestDoubleToBigToFloat)
		{
			try
			{
				double x = 1.2e100;
				float result = narrow_cast<float>(x);
				Assert::Fail(L"Narrow cast 1.2e100 to float should have failed", LINE_INFO());
			}
			catch (bad_narrow_cast&)
			{
			}
		}

	};
}