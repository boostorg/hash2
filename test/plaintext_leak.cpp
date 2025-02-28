// Copyright 2017, 2018 Peter Dimov.
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/hash2/fnv1a.hpp>
#include <boost/hash2/siphash.hpp>
#include <boost/hash2/xxhash.hpp>
#include <boost/hash2/md5.hpp>
#include <boost/hash2/sha1.hpp>
#include <boost/hash2/sha2.hpp>
#include <boost/hash2/sha3.hpp>
#include <boost/hash2/ripemd.hpp>
#include <boost/hash2/blake2.hpp>
#include <boost/core/lightweight_test.hpp>
#include <algorithm>
#include <cstddef>

template<class H> void test( bool is_blake2 = false )
{
    char const * s = "xxxx";

    {
        H h;

        h.update( s, 4 );

        h.result();

        unsigned char const * p = reinterpret_cast<unsigned char const*>( &h );
        std::size_t n = sizeof( h );

        BOOST_TEST_EQ( std::search( p, p + n, s, s + 4 ) - p, n );
    }

    {
        H h;

        unsigned char buffer[ 1024 ] = {};
        h.update( buffer, 1024 );

        h.update( s, 4 );

        h.result();

        unsigned char const * p = reinterpret_cast<unsigned char const*>( &h );
        std::size_t n = sizeof( h );

        BOOST_TEST_EQ( std::search( p, p + n, s, s + 4 ) - p, n );
    }

    if( !is_blake2 )
    {
        // A 4 byte seed is sufficient to be treated as keyed construction for BLAKE2.
        // This means that the internal buffer will contain the seed here as it's
        // incorrect to transform the input without knowing if we have the last block
        // or not.
        // https://datatracker.ietf.org/doc/html/rfc7693#section-3.3

        H h( s, 4 );

        unsigned char const * p = reinterpret_cast<unsigned char const*>( &h );
        std::size_t n = sizeof( h );

        BOOST_TEST_EQ( std::search( p, p + n, s, s + 4 ) - p, n );
    }
}

int main()
{
    test<boost::hash2::fnv1a_32>();
    test<boost::hash2::fnv1a_64>();
    test<boost::hash2::xxhash_32>();
    test<boost::hash2::xxhash_64>();
    test<boost::hash2::siphash_32>();
    test<boost::hash2::siphash_64>();

    test<boost::hash2::md5_128>();
    test<boost::hash2::sha1_160>();
    test<boost::hash2::sha2_256>();
    test<boost::hash2::sha2_224>();
    test<boost::hash2::sha2_512>();
    test<boost::hash2::sha2_384>();
    test<boost::hash2::sha2_512_224>();
    test<boost::hash2::sha2_512_256>();
    test<boost::hash2::sha3_256>();
    test<boost::hash2::sha3_224>();
    test<boost::hash2::sha3_512>();
    test<boost::hash2::sha3_384>();
    test<boost::hash2::shake_128>();
    test<boost::hash2::shake_256>();
    test<boost::hash2::ripemd_160>();
    test<boost::hash2::ripemd_128>();
    test<boost::hash2::blake2b_512>( true );
    test<boost::hash2::blake2s_256>( true );

    test<boost::hash2::hmac_md5_128>();
    test<boost::hash2::hmac_sha1_160>();
    test<boost::hash2::hmac_sha2_256>();
    test<boost::hash2::hmac_sha2_224>();
    test<boost::hash2::hmac_sha2_512>();
    test<boost::hash2::hmac_sha2_384>();
    test<boost::hash2::hmac_sha2_512_224>();
    test<boost::hash2::hmac_sha2_512_256>();
    test<boost::hash2::hmac_sha3_256>();
    test<boost::hash2::hmac_sha3_224>();
    test<boost::hash2::hmac_sha3_512>();
    test<boost::hash2::hmac_sha3_384>();
    test<boost::hash2::hmac_ripemd_160>();
    test<boost::hash2::hmac_ripemd_128>();

    return boost::report_errors();
}
