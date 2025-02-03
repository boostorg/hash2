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
        TEST_EQ( test<sha3_384>( 7, str1 ), digest_from_hex( "c16d70af9d4e75ad91fd0dfec80e52bd2dbad910d5afc36b424b5280755409e61a117a937a5872ccfd228b42a0153dde" ) );

#if !BOOST_WORKAROUND(BOOST_MSVC, < 1920)

        constexpr char const str2[] = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

        TEST_EQ( test<sha3_384>( 0, str2 ), digest_from_hex( "79407d3b5916b59c3e30b09822974791c313fb9ecc849e406f23592d04f625dc8c709b98b43b3852b337216179aa7fc7" ) );
        TEST_EQ( test<sha3_384>( 7, str2 ), digest_from_hex( "01d50e968b05de4a3f35eb9c2ee626fb16c81d1c55d0a1cfdc1e5170ed33e8eb8a907f32614ccb3345df600f7f02ed1a" ) );

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

        TEST_EQ( test<sha3_384>( 7, buf1 ), digest_from_hex( "a6b123e7c5fc9ea18a37426cdddc2ce1da169d043a03e3a792197d267ab91ef46dacbff9915665c98aa0884a6fc48933" ) );
        TEST_EQ( test<sha3_384>( 7, buf2 ), digest_from_hex( "db643f4f3155e5a88edd9c1a180bc81c6589e247511c74487c9703747a7372a2d820600dd1aeece26a61d5da9d164dd1" ) );
        TEST_EQ( test<sha3_384>( 7, buf3 ), digest_from_hex( "bc0ee67efc7b020ef5f399ca88bcde73238a0fee550e3a017f824acd8762c68135315a33963a035bb0b6d6aca244c61e" ) );
        TEST_EQ( test<sha3_384>( 7, buf4 ), digest_from_hex( "bfc62bcfc4ed7e98f09e0679f3bfdcd5d3b0acd02dc62df040f8b74d838cf1b6167a645baad5d8facc4bd882effc1dee" ) );
        TEST_EQ( test<sha3_384>( 7, buf5 ), digest_from_hex( "5f6eea5e7f975144e4848e3aae0c2e12b9e95823538b26b5585088ca36c61db03598f95980b2965346b09b383dbe55d5" ) );

#endif
    }

    return boost::report_errors();
}
