
#include "stdafx.h"
#include "DxException.h"

namespace DXF
{
	DxException::DxException(char const* const _Message, long hr) throw()
		: std::exception(_Message)
		, m_hr(hr)
	{}

	DxException::DxException(const std::string& msg, long hr) throw()
		: std::exception(msg.c_str())
		, m_hr(hr)
	{}

	DxException::DxException(const DxException& ex) throw()
		: std::exception(ex.what())
		, m_hr(ex.m_hr)
	{}

	DxException::~DxException()
	{}

	long DxException::GetHr()
	{
		return m_hr;
	}
}