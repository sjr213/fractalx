#pragma once

#include <typeinfo>
#include <sstream> 

// adapted from
// https://codereview.stackexchange.com/questions/231364/implementation-of-narrow-cast-in-c
namespace DxCore
{
	class bad_narrow_cast : public std::bad_cast
	{
	public:
		bad_narrow_cast()
		{}

		bad_narrow_cast(char const* message) : what_(message)
		{}

		char const* what() { return what_; }

	private:
		char const* what_;
	};

	template <typename ToType, typename FromType> static inline
		ToType narrow_cast(FromType const& from)
	{
		ToType to = static_cast<ToType>(from);
		FromType narrowed_base = static_cast<FromType>(to);

		if (from == narrowed_base)
			return to;

		std::stringstream ss;
		ss << "Invalid narrowing conversation from type ";
		ss << typeid(FromType).name();
		ss << " to type ";
		ss << typeid(ToType).name();
		throw bad_narrow_cast(ss.str().c_str());
	}
}