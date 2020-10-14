#pragma once

// Thrown when serialization fails

namespace DxCore
{
	class CSerializationException : public std::exception
	{
	public:
		CSerializationException(char const* const _Message) throw();

		CSerializationException(const std::string& msg) throw();

		CSerializationException(const CSerializationException& ex) throw();

		virtual ~CSerializationException();
	};
}

