#pragma once

#include <typeinfo>
#include <sstream> 

namespace DxCore
{
	class narrowing_error : public std::bad_cast
	{
	public:
		narrowing_error() : what_("cast failed")
		{}

		narrowing_error(char const* message) : what_(message)
		{}

		char const* what() { return what_; }

	private:
		char const* what_;
	};

	
	// adapted from
	// https://codereview.stackexchange.com/questions/231364/implementation-of-narrow-cast-in-c
	/*
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
		throw narrowing_error(ss.str().c_str());
	}
	*/

	template <typename T, typename U>
	bool AreSameSign()
	{
		return (std::is_signed<T>::value == std::is_signed<U>::value);
	}

	// narrow_cast(): a searchable way to do narrowing casts of values
	template <typename T, typename U>
	constexpr T narrow_cast(U&& u) noexcept
	{
		return static_cast<T>(std::forward<U>(u));
	}

	// narrow() : a checked version of narrow_cast() that throws if the cast changed the value
	// Don't use for double -> float
	template <typename T, typename U>
	T narrow(U u) noexcept(false)
	{
		T t = narrow_cast<T>(u);
		if (static_cast<U>(t) != u)
			throw narrowing_error();
		if (!AreSameSign<T,U>() && ((t < T{}) != (u < U{})))  // <-- ???
			throw narrowing_error();
		return t;
	}
}