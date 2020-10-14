#include "stdafx.h"
#include "SerializationException.h"

namespace DxCore
{
	CSerializationException::CSerializationException(char const* const _Message) throw()
		: std::exception(_Message)
	{}

	CSerializationException::CSerializationException(const std::string& msg) throw()
		: std::exception(msg.c_str())
	{}

	CSerializationException::CSerializationException(const CSerializationException& ex) throw()
		: std::exception(ex.what())
	{}

	CSerializationException::~CSerializationException()
	{}
}