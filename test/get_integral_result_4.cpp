// Copyright 2017, 2018, 2024 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/hash2/get_integral_result.hpp>
#include <boost/hash2/fnv1a.hpp>
#include <boost/hash2/hash_append.hpp>
#include <boost/core/lightweight_test.hpp>
#include <set>
#include <limits>
#include <cstdint>

template<class Hash> void test_identity()
{
    using boost::hash2::get_integral_result;

    using R = typename Hash::result_type;

    Hash h;
    Hash h2( h );

    for( int i = 0; i < 1024; ++i )
    {
        R r = h.result();
        R t = get_integral_result<R>( h2 );

        BOOST_TEST_EQ( t, r );
    }
}

template<class T, class Hash> std::size_t test_sample()
{
    using boost::hash2::get_integral_result;

    std::set<T> dist;

    for( unsigned i = 0; i <= std::numeric_limits<T>::max(); ++i )
    {
        T t1 = static_cast<T>( i );

        Hash h;
        boost::hash2::hash_append( h, {}, t1 );

        T t2 = get_integral_result<T>( h );

        dist.insert( t2 );
    }

    return dist.size();
}

template<class R> struct H1: private boost::hash2::fnv1a_64
{
    using result_type = R;

    using boost::hash2::fnv1a_64::update;

    result_type result()
    {
        return static_cast<R>( boost::hash2::fnv1a_64::result() );
    }
};

int main()
{
    test_identity< H1<std::uint8_t> >();

    BOOST_TEST_EQ( (test_sample<std::uint8_t, H1<std::uint8_t>>()), 256u );
    BOOST_TEST_GE( (test_sample<std::uint8_t, H1<std::uint16_t>>()), 191u ); // !
    BOOST_TEST_GE( (test_sample<std::uint8_t, boost::hash2::fnv1a_32>()), 64u ); // !!
    BOOST_TEST_GE( (test_sample<std::uint8_t, boost::hash2::fnv1a_64>()), 255u ); // !

    test_identity< H1<std::uint16_t> >();

    BOOST_TEST_GE( (test_sample<std::uint16_t, boost::hash2::fnv1a_32>()), 46414u ); // !
    BOOST_TEST_GE( (test_sample<std::uint16_t, boost::hash2::fnv1a_64>()), 47196u ); // !

    return boost::report_errors();
}
