#pragma once

#include <vcruntime_exception.h>

class CCancelException : public std::exception
{
public:
	CCancelException(char const* const _Message) throw();

	CCancelException(const std::string& msg) throw();

	CCancelException(const CCancelException& ex) throw();

	virtual ~CCancelException() throw();
};
