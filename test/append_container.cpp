// Copyright 2017, 2024 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/hash2/hash_append.hpp>
#include <boost/hash2/fnv1a.hpp>
#include <boost/core/lightweight_test.hpp>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <forward_list>

template<class Hash, class Flavor, class T, class R> void test_container( R r )
{
    unsigned char w[] = { 1, 2, 3, 4 };
    T v( w, w + sizeof(w) / sizeof(w[0]) );

    Flavor f;

    {
        Hash h;
        boost::hash2::hash_append( h, f, v );

        BOOST_TEST_EQ( h.result(), r );
    }

    {
        Hash h;
        boost::hash2::hash_append_range_and_size( h, f, v.begin(), v.end() );

        BOOST_TEST_EQ( h.result(), r );
    }
}

template<class Hash, class Flavor, class T, class R> void test( R r )
{
    test_container< Hash, Flavor, std::vector<T> >( r );
    test_container< Hash, Flavor, std::list<T> >( r );
    test_container< Hash, Flavor, std::deque<T> >( r );
    test_container< Hash, Flavor, std::set<T> >( r );
    test_container< Hash, Flavor, std::multiset<T> >( r );
    test_container< Hash, Flavor, std::forward_list<T> >( r );
}

int main()
{
    using namespace boost::hash2;

    test<fnv1a_32, little_endian_flavor, unsigned char>( 2468847257 );
    test<fnv1a_32, big_endian_flavor, unsigned char>( 78451921 );

    test<fnv1a_32, little_endian_flavor, int>( 2374676325 );
    test<fnv1a_32, big_endian_flavor, int>( 2877134693 );

    test<fnv1a_32, little_endian_flavor, double>( 461647460 );
    test<fnv1a_32, big_endian_flavor, double>( 1394753398 );

    return boost::report_errors();
}
