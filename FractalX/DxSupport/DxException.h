#pragma once
#include <vcruntime_exception.h>
#include <string>

namespace DXF
{
	class DxException : public std::exception
	{
	public:
		DxException(char const* const _Message, long hr) throw();

		DxException(const std::string& msg, long hr) throw();

		DxException(const DxException& ex) throw();

		virtual ~DxException() throw();

		long GetHr();

	private:
		long m_hr;
	};

	inline void ThrowIfFailure(HRESULT hr, std::string msg)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch DirectX API errors
			throw DxException(msg, hr);
		}
	}
}