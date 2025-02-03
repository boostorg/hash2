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

        TEST_EQ( test<sha3_512>( 0, str1 ), digest_from_hex( "b751850b1a57168a5693cd924b6b096e08f621827444f70d884f5d0240d2712e10e116e9192af3c91a7ec57647e3934057340b4cf408d5a56592f8274eec53f0" ) );
        TEST_EQ( test<sha3_512>( 7, str1 ), digest_from_hex( "7fcdaa301f16df33e622e204cbfda936928e7b23ecd26a4efdb30c11782b7dfb079d45800015a8ca306a95ad5bc97c3aa7ed99920e1730826d8260fad8a59d33" ) );

#if !BOOST_WORKAROUND(BOOST_MSVC, < 1920)

        constexpr char const str2[] = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

        TEST_EQ( test<sha3_512>( 0, str2 ), digest_from_hex( "afebb2ef542e6579c50cad06d2e578f9f8dd6881d7dc824d26360feebf18a4fa73e3261122948efcfd492e74e82e2189ed0fb440d187f382270cb455f21dd185" ) );
        TEST_EQ( test<sha3_512>( 7, str2 ), digest_from_hex( "fc4a2170b4693ae60039188d07e8d49d9b4457fc42e86f3773baa123d48b5fb655bcee8d7371fa136f4dcecdfac647e7275c42b8cc96a616ce67c8f4f3cac66e" ) );

        constexpr unsigned char buf1[ 111 ] = {};
        constexpr unsigned char buf2[ 112 ] = {};
        constexpr unsigned char buf3[ 113 ] = {};
        constexpr unsigned char buf4[ 122 ] = {};
        constexpr unsigned char buf5[ 1000 ] = {};

        TEST_EQ( test<sha3_512>( 0, buf1 ), digest_from_hex( "c677adbab492fd76bff50e41bddae49b9c8fe0b47801a73a47632d8e895ccdc631e7f00f7f87fa5baec2ab86e47ea07072c829bdfe36f251e57d7f5507ff9d06" ) );
        TEST_EQ( test<sha3_512>( 0, buf2 ), digest_from_hex( "af8bd43e6f05532448f6509151a871db4ddd867f386aadb2b553a75e30f81bd3c2c79eec5cde15b0ef9be399ab166f4e1f2620934e584223d9f9504641a10f24" ) );
        TEST_EQ( test<sha3_512>( 0, buf3 ), digest_from_hex( "d501c1503b8ef0846ef4fef2a3383a98957296879a6047f3c9adea670666bdf8ba1c46283d99433bafab4df59f16da32fd66f1a43f7f4664b696635c92333e2c" ) );
        TEST_EQ( test<sha3_512>( 0, buf4 ), digest_from_hex( "017d18ab7042eacc42bcf9f100a2ba45a14594434e76b3e2c4c59bb7710b13e99d6388d2542d7f0318bb8e0467fe46f8a1277c6d26db59f09f28e46fa8b60815" ) );
        TEST_EQ( test<sha3_512>( 0, buf5 ), digest_from_hex( "422d49f19849352a41f4a61796feb28c18a79883fdb2cc52df63dc7cf0b13efddd757fac95861403b1cca3f28cb164f897df9bbfc2f579567d10f8fb21b5c6bf" ) );

        TEST_EQ( test<sha3_512>( 7, buf1 ), digest_from_hex( "2dead5a91b841bb750f89f6057a0d988940a68bfc6261105ce8f6d059add89169526d994339aa1f563af96e26c72e2ac5fb9d5125b5197cc214c1451bf449856" ) );
        TEST_EQ( test<sha3_512>( 7, buf2 ), digest_from_hex( "ba654b58fb1204036dac6bc97cbabad90409561aa0c77515fbb9130754df4ac5318cab76e581f44bfc07b973af84a29734c93ba9e87a20ce087eb282c14f42e9" ) );
        TEST_EQ( test<sha3_512>( 7, buf3 ), digest_from_hex( "705069ae85d9d02f2ec042adede304624ce7083ac2bd3611640f0073e0d0ca8d80b921a31b3cdba1632941127adada7b17075500d3e5e8b1cf308445be95f55d" ) );
        TEST_EQ( test<sha3_512>( 7, buf4 ), digest_from_hex( "c72315a5d1eed9d02c67d523728d9ede920b19676cd4b619bf5049a236d6d0db636c3ed30fc544e7e94917f266fad09f010272c27704d9f0a20d532b5259c3ef" ) );
        TEST_EQ( test<sha3_512>( 7, buf5 ), digest_from_hex( "b0604e689246d693890ab616ccbdc4fe39a6d6763cc74f10e10142205cd5c6f1c66beae15a4b601dcf00f868b82d375a21f2cd57e51d870a897f7f0b1995de37" ) );

#endif
    }

    return boost::report_errors();
}
