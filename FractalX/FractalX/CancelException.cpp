
#include "stdafx.h"
#include "CancelException.h"

CCancelException::CCancelException(char const* const _Message) throw()
	: std::exception(_Message)
{}

CCancelException::CCancelException(const std::string& msg) throw()
	: std::exception(msg.c_str())
{}

CCancelException::CCancelException(const CCancelException& ex) throw()
	: std::exception(ex.what())
{}

CCancelException::~CCancelException() throw()
{}