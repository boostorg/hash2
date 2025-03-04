#ifndef BOOST_HASH2_GET_INTEGRAL_RESULT_HPP_INCLUDED
#define BOOST_HASH2_GET_INTEGRAL_RESULT_HPP_INCLUDED

// Copyright 2017, 2018 Peter Dimov.
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/hash2/detail/read.hpp>
#include <type_traits>
#include <limits>
#include <cstddef>

namespace boost
{
namespace hash2
{

namespace detail
{

// identity

template<class U, class R>
    constexpr typename std::enable_if<sizeof(R) == sizeof(U), U>::type
    get_result_multiplier()
{
    return 1;
}

// contraction

// 2 -> 1
template<class U, class R>
    constexpr typename std::enable_if<sizeof(R) == 2 && sizeof(U) == 1, R>::type
    get_result_multiplier()
{
    return 0xBF01;
}

// 4 -> 1
template<class U, class R>
    constexpr typename std::enable_if<sizeof(R) == 4 && sizeof(U) == 1, R>::type
    get_result_multiplier()
{
    return 0x7F7F7F7Fu;
}

// 8 -> 1
template<class U, class R>
    constexpr typename std::enable_if<sizeof(R) == 8 && sizeof(U) == 1, R>::type
    get_result_multiplier()
{
    return 0x7F7F7F7F7F7F7F7Full;
}

// 4 -> 2
template<class U, class R>
    constexpr typename std::enable_if<sizeof(R) == 4 && sizeof(U) == 2, R>::type
    get_result_multiplier()
{
    return 0xBFFF0001u;
}

// 8 -> 2
template<class U, class R>
    constexpr typename std::enable_if<sizeof(R) == 8 && sizeof(U) == 2, R>::type
    get_result_multiplier()
{
    return 0xBFFFBFFFBFFFBFFFull;
}

// 8 -> 4
template<class U, class R>
    constexpr typename std::enable_if<sizeof(R) == 8 && sizeof(U) == 4, R>::type
    get_result_multiplier()
{
    return 0xBFFFFFFF00000001ull;
}

// expansion

// 1 -> 2
template<class U, class R>
    constexpr typename std::enable_if<sizeof(R) == 1 && sizeof(U) == 2, U>::type
    get_result_multiplier()
{
    return 0x7F7F;
}

// 1 -> 4
template<class U, class R>
    constexpr typename std::enable_if<sizeof(R) == 1 && sizeof(U) == 4, U>::type
    get_result_multiplier()
{
    return 0xBFBFBFBFu;
}

// 1 -> 8
template<class U, class R>
    constexpr typename std::enable_if<sizeof(R) == 1 && sizeof(U) == 8, U>::type
    get_result_multiplier()
{
    return 0xDFDFDFDFDFDFDFDFull;
}

// 2 -> 4
template<class U, class R>
    constexpr typename std::enable_if<sizeof(R) == 2 && sizeof(U) == 4, U>::type
    get_result_multiplier()
{
    return 0x7FFF7FFFu;
}

// 2 -> 8
template<class U, class R>
    constexpr typename std::enable_if<sizeof(R) == 2 && sizeof(U) == 8, U>::type
    get_result_multiplier()
{
    return 0x7FFF7FFF7FFF7FFFull;
}

// 4 -> 8
template<class U, class R>
    constexpr typename std::enable_if<sizeof(R) == 4 && sizeof(U) == 8, U>::type
    get_result_multiplier()
{
    return 0x7FFFFFFF7FFFFFFFull;
}

} // namespace detail

// contraction

template<class T, class Hash, class R = typename Hash::result_type>
    typename std::enable_if<std::is_integral<R>::value && (sizeof(R) > sizeof(T)), T>::type
    get_integral_result( Hash& h )
{
    static_assert( std::is_integral<T>::value, "T must be integral" );
    static_assert( !std::is_same<typename std::remove_cv<T>::type, bool>::value, "T must not be bool" );

    static_assert( std::is_unsigned<R>::value, "Hash::result_type must be unsigned" );

    typedef typename std::make_unsigned<T>::type U;

    constexpr auto m = detail::get_result_multiplier<U, R>();

    auto r = h.result();
    return static_cast<T>( static_cast<U>( ( r * m ) >> ( std::numeric_limits<R>::digits - std::numeric_limits<U>::digits ) ) );
}

// identity or expansion

template<class T, class Hash, class R = typename Hash::result_type>
    typename std::enable_if<std::is_integral<R>::value && (sizeof(R) <= sizeof(T)), T>::type
    get_integral_result( Hash& h )
{
    static_assert( std::is_integral<T>::value, "T must be integral" );
    static_assert( !std::is_same<typename std::remove_cv<T>::type, bool>::value, "T must not be bool" );

    static_assert( std::is_unsigned<R>::value, "Hash::result_type must be unsigned" );

    typedef typename std::make_unsigned<T>::type U;

    constexpr auto m = detail::get_result_multiplier<U, R>();

    auto r = h.result();
    return static_cast<T>( static_cast<U>( r * m ) );
}

// array-like R

template<class T, class Hash, class R = typename Hash::result_type>
    typename std::enable_if< !std::is_integral<R>::value, T >::type
    get_integral_result( Hash& h )
{
    static_assert( std::is_integral<T>::value, "T must be integral" );
    static_assert( !std::is_same<typename std::remove_cv<T>::type, bool>::value, "T must not be bool" );

    static_assert( R().size() >= 8, "Array-like result type is too short" );

    auto r = h.result();
    return static_cast<T>( detail::read64le( r.data() ) );
}

} // namespace hash2
} // namespace boost

#endif // #ifndef BOOST_HASH2_GET_INTEGRAL_RESULT_HPP_INCLUDED
