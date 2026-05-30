// Copyright 2017, 2024, 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/hash2/hash_append.hpp>
#include <boost/hash2/fnv1a.hpp>
#include <boost/config.hpp>
#include <boost/config/pragma_message.hpp>

#if defined(BOOST_NO_CXX17_HDR_VARIANT)

BOOST_PRAGMA_MESSAGE( "Test skipped because BOOST_NO_CXX17_HDR_VARIANT is defined" )
int main() {}

#else

#include <boost/core/lightweight_test.hpp>
#include <variant>

template<class Hash, class Flavor, class... T, class R> void test( std::variant<T...> const& v, R r )
{
    Flavor f;

    {
        Hash h;

        hash_append( h, f, v );

        BOOST_TEST_EQ( h.result(), r );
    }

    if( v.valueless_by_exception() )
    {
        Hash h;

        hash_append( h, f, '\x00' );

        BOOST_TEST_EQ( h.result(), r );
    }
    else
    {
        Hash h;

        hash_append_size( h, f, v.index() );
        std::visit( [&]( auto const& w ){ hash_append( h, f, w ); }, v );

        BOOST_TEST_EQ( h.result(), r );
    }
}

int main()
{
    using namespace boost::hash2;

    {
        std::variant<char, int, double> v( 'x' );

        test<fnv1a_32, little_endian_flavor>( v, 1779522199 );
        test<fnv1a_32, big_endian_flavor>( v, 1779522199 );
    }

    {
        std::variant<char, int, double> v( 5 );

        test<fnv1a_32, little_endian_flavor>( v, 1308316945 );
        test<fnv1a_32, big_endian_flavor>( v, 1379542249 );
    }

    {
        std::variant<char, int, double> v( 3.14 );

        test<fnv1a_32, little_endian_flavor>( v, 2633093848 );
        test<fnv1a_32, big_endian_flavor>( v, 1046089386 );
    }

    return boost::report_errors();
}

#endif
