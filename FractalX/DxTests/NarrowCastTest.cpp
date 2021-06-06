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
		TEST_METHOD(TestAreSameSignInts)
		{
			bool result = AreSameSign<int, int>();
			Assert::IsTrue(result, L"int and int should be the same sign", LINE_INFO());
		}

		TEST_METHOD(TestAreSameSignIntUnsignedInt)
		{
			bool result = AreSameSign<int, unsigned int>();
			Assert::IsFalse(result, L"int and unsigned int should not be the same sign", LINE_INFO());
		}

		TEST_METHOD(TestAreSameSignIntInt8)
		{
			bool result = AreSameSign<int, __int8>();
			Assert::IsTrue(result, L"int and __int8 should be the same sign", LINE_INFO());
		}

		TEST_METHOD(TestAreSameSignIntUInt8)
		{
			bool result = AreSameSign<int, unsigned __int8>();
			Assert::IsFalse(result, L"int and unsigned __int8 should not be the same sign", LINE_INFO());
		}

		TEST_METHOD(TestAreSameSignDoubleFloat)
		{
			bool result = AreSameSign<double, float>();
			Assert::IsTrue(result, L"double and float should be the same sign", LINE_INFO());
		}

		TEST_METHOD(TestNarrowIntToBite)
		{
			try
			{
				int x = 10;
				unsigned __int8 result = narrow<unsigned __int8>(x);

				Assert::AreEqual(result, 10ui8, L"Narrow 10 to bite failed", LINE_INFO());
			}
			catch (narrowing_error&)
			{
				Assert::Fail(L"Narrow int 10 to __int8 threw", LINE_INFO());
			}
		}

		TEST_METHOD(TestNarrowIntToBiteShouldFail)
		{
			try
			{
				int x = 300;
				unsigned __int8 result = narrow<unsigned __int8>(x);
				Assert::Fail(L"Narrow int 300 to __int8 did not throw?", LINE_INFO());
			}
			catch (narrowing_error&)
			{
				// expect it to throw
			}
		}

		TEST_METHOD(TestNarrowIntToUInt)
		{
			try
			{
				int x = 500;
				unsigned int result = narrow<unsigned int>(x);
				Assert::AreEqual(500u, result, L"Narrow 500 to uint should not fail", LINE_INFO());
			}
			catch (narrowing_error&)
			{
				Assert::Fail(L"Narrow 500 to uint should not throw", LINE_INFO());
			}
		}

		TEST_METHOD(TestNarrowNegativeIntToUInt)
		{
			try
			{
				int x = -500;
				unsigned int result = narrow<unsigned int>(x);
				Assert::Fail(L"Narrow -500 to uint should throw", LINE_INFO());
			}
			catch (narrowing_error&)
			{

			}
		}

		TEST_METHOD(TestNarrowCastDoubleToFloat)
		{
			try
			{
				double x = 3.14;
				float result = narrow_cast<float>(x);
				Assert::AreEqual(result, 3.14f, 0.00001f, L"Narrow cast 3.14 to float should have passed", LINE_INFO());
			}
			catch (narrowing_error&)
			{
				Assert::Fail(L"Narrow 3.14 to float should not throw", LINE_INFO());
			}
		}

	};
}