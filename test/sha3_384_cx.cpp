// Copyright 2024 Christian Mazakas
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/hash2/sha3.hpp>
#include <boost/hash2/digest.hpp>
#include <boost/hash2/detail/config.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/config.hpp>
#include <boost/config/workaround.hpp>

#if defined(BOOST_MSVC) && BOOST_MSVC < 1920
# pragma warning(disable: 4307) // integral constant overflow
#endif

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

// gcc-5 has issues with these constexpr tests:
// libs/hash2/test/sha3_cx.cpp:100:132: internal compiler error: in fold_binary_loc, at fold-const.c:9925
//          TEST_EQ( test<sha3_256>( 0, str1 ), digest_from_hex( "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532" ) );
#if defined(BOOST_NO_CXX14_CONSTEXPR) || BOOST_WORKAROUND(BOOST_GCC, < 60000)
# define TEST_EQ(x1, x2) BOOST_TEST_EQ(x1, x2)
#else
# define TEST_EQ(x1, x2) BOOST_TEST_EQ(x1, x2); STATIC_ASSERT( x1 == x2 )
#endif

template<class H, std::size_t N> BOOST_CXX14_CONSTEXPR typename H::result_type test( std::uint64_t seed, char const (&str)[ N ] )
{
    H h( seed );

    std::size_t const M = N - 1; // strip off null-terminator

    // only update( unsigned char const*, std::size_t ) is constexpr so we memcpy here to emulate bit_cast
    unsigned char buf[M] = {};
    for( unsigned i = 0; i < M; ++i ){ buf[i] = str[i]; }

    h.update( buf, M / 3 );
    h.update( buf + M / 3, M - M / 3 );

    return h.result();
}

template<class H, std::size_t N> BOOST_CXX14_CONSTEXPR typename H::result_type test( std::uint64_t seed, unsigned char const (&buf)[ N ] )
{
    H h( seed );

    h.update( buf, N / 3 );
    h.update( buf + N / 3, N - N / 3 );

    return h.result();
}


BOOST_CXX14_CONSTEXPR unsigned char to_byte( char c )
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0xff;
}

template<std::size_t N, std::size_t M = ( N - 1 ) / 2>
BOOST_CXX14_CONSTEXPR boost::hash2::digest<M> digest_from_hex( char const (&str)[ N ] )
{
    boost::hash2::digest<M> dgst = {};
    auto* p = dgst.data();
    for( unsigned i = 0; i < M; ++i ) {
        auto c1 = to_byte( str[ 2 * i ] );
        auto c2 = to_byte( str[ 2 * i + 1 ] );
        p[ i ] = ( c1 << 4 ) | c2;
    }
    return dgst;
}

template<class H, std::size_t N> BOOST_CXX14_CONSTEXPR typename H::result_type test_hex( std::uint64_t seed, char const (&str)[ N ] )
{
    H h( seed );

    std::size_t const M = ( N - 1 ) / 2;

    unsigned char buf[M] = {};
    for( unsigned i = 0; i < M; ++i ) {
        auto c1 = to_byte( str[ 2 * i ] );
        auto c2 = to_byte( str[ 2 * i + 1 ] );
        buf[ i ] = ( c1 << 4 ) | c2;
    }

    h.update( buf, M / 3 );
    h.update( buf + M / 3, M - M / 3 );

    return h.result();
}

int main()
{
    using namespace boost::hash2;

    {
        constexpr char const str1[] = "abc";

        TEST_EQ( test<sha3_384>( 0, str1 ), digest_from_hex( "ec01498288516fc926459f58e2c6ad8df9b473cb0fc08c2596da7cf0e49be4b298d88cea927ac7f539f1edf228376d25" ) );
        TEST_EQ( test<sha3_384>( 7, str1 ), digest_from_hex( "8a9b1a3f5e568ccd6c2a6a07ba3544104b647da53f0d4f1958d434c09772ce96aa1995fe08c76fc9d8a18339c805d1cc" ) );

#if !BOOST_WORKAROUND(BOOST_MSVC, < 1920)

        constexpr char const str2[] = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

        TEST_EQ( test<sha3_384>( 0, str2 ), digest_from_hex( "79407d3b5916b59c3e30b09822974791c313fb9ecc849e406f23592d04f625dc8c709b98b43b3852b337216179aa7fc7" ) );
        TEST_EQ( test<sha3_384>( 7, str2 ), digest_from_hex( "a2f38501b0cc49e244cbabfc100297ec5d0671937caa7ab891ac657eaf3f553e39bdb0f073fc8c35235556827029fbb3" ) );

        constexpr unsigned char buf1[ 111 ] = {};
        constexpr unsigned char buf2[ 112 ] = {};
        constexpr unsigned char buf3[ 113 ] = {};
        constexpr unsigned char buf4[ 122 ] = {};
        constexpr unsigned char buf5[ 1000 ] = {};

        TEST_EQ( test<sha3_384>( 0, buf1 ), digest_from_hex( "76578d4cc96f5abaa7b7676d75685eb58e3a9bae3574f5d4f208d7d16cfd251e34ef1cc673a3ee21c1511a1fedf4902b" ) );
        TEST_EQ( test<sha3_384>( 0, buf2 ), digest_from_hex( "5553d3b7829478df436c8f1c990144627413b2dc7da0a19e019ab249349f57dd29873c82b04066252a76933cf4c20c32" ) );
        TEST_EQ( test<sha3_384>( 0, buf3 ), digest_from_hex( "069e27853643a04ee85f1515f811e8b77697a78d155fb2c6c65cf1d86d84eb8c6af4639505f98c72d1ebdfa6da8fddbe" ) );
        TEST_EQ( test<sha3_384>( 0, buf4 ), digest_from_hex( "cd6131e2e00a6961663469d371de6f653c3df25e792575461ba5add1bbb0669410fff5e0b8e39e24af91821cbb2fb4e6" ) );
        TEST_EQ( test<sha3_384>( 0, buf5 ), digest_from_hex( "32bcf4f306fadc00701becdcf0c7f44134a998f1cfcd9b9af1df04ac90095b958fda27ef7e6638572b6df5478b99365d" ) );

        TEST_EQ( test<sha3_384>( 7, buf1 ), digest_from_hex( "6f5026363c4913655b545523833d4b2ca835e011225c241e7dfd409bf7f5bdaf676d2a133b2609fdf8ef988e71b2da45" ) );
        TEST_EQ( test<sha3_384>( 7, buf2 ), digest_from_hex( "6ff218f67b58d7ffb41bba299f61378ffdf09d190e4a10c99e2d77c308b9cc644383050591f577ae124f03b53da70366" ) );
        TEST_EQ( test<sha3_384>( 7, buf3 ), digest_from_hex( "b8caa3113e315b2a1fd69ffc162ef13b9eeaf8e3ea7e02e1c652bc250d0ece626b044901efc294f1cb182a1804a34c0e" ) );
        TEST_EQ( test<sha3_384>( 7, buf4 ), digest_from_hex( "bdbcdee009105091b6f755a16320dec2969a473f905e45b32ba6c83c6b2499c21c54bc9e75a331d10af71f0535296ef2" ) );
        TEST_EQ( test<sha3_384>( 7, buf5 ), digest_from_hex( "b62d536e16301d01fe5ab227bd3f27c78fef6e4bd8c1924a627e5244f0d4a0f6daaa1607525095736b050d1deec34f5a" ) );

#endif
    }

    return boost::report_errors();
}
