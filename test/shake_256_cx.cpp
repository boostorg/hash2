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
        constexpr char const str2[] = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

        constexpr char const buf1[] = "fa";
        constexpr char const buf2[] = "74e4";
        constexpr char const buf3[] = "6f63b4";
        constexpr char const buf4[] = "7dae8fc020d9";
        constexpr char const buf5[] = "8d2782a7843aa477b8f3bca9f9f2bafb5813db4c8c43";
        constexpr char const buf6[] = "95de55287ad3cff69efec6e97c812456e47be25e433470c3259b";
        constexpr char const buf7[] = "1b77c8dcfd2fc4b54617054fa6b14d6e9d09ce9185a34a7fd2b27923998aab99";
        constexpr char const buf8[] = "a52dde5aea1f04399c5d91fb5c4e62da06b73a5d9bdc5a927fe85298e58e166187f154b69ac1057c01a7";

        TEST_EQ( test<shake_256>( 0, str1 ), digest_from_hex( "483366601360a8771c6863080cc4114d8db44530f8f1e1ee4f94ea37e78b5739d5a15bef186a5386c75744c0527e1faa9f8726e462a12a4feb06bd8801e751e41385141204f329979fd3047a13c5657724ada64d2470157b3cdc288620944d78dbcddbd912993f0913f164fb2ce95131a2d09a3e6d51cbfc622720d7a75c6334e8a2d7ec71a7cc29" ) );
        TEST_EQ( test<shake_256>( 0, str2 ), digest_from_hex( "98be04516c04cc73593fef3ed0352ea9f6443942d6950e29a372a681c3deaf4535423709b02843948684e029010badcc0acd8303fc85fdad3eabf4f78cae165635f57afd28810fc22abf63df55c5ead450fdfb64209010e982102aa0b5f0a4b4753b53eb4b5319c06986f5aac5cc247256d06b05a273d7ef8d31864777d488d541451ed82a389265" ) );

        TEST_EQ( test_hex<shake_256>( 0, buf1 ), digest_from_hex( "6a22064c097e8e8063c1d6f9aa8b1c1a4da72709afbc3c5d776c5d2372a3c539b7fdaebbe7a13a4650fdd9c6e0e121b8d34021200156279da85dbf414206f73e98e146e86f19c5bb91806dcf737e3b76e2f2ec08517288e6e60e8dd47776c259db2dcf28cf44ab481114f68a2a39306fb608cd2f383dff81a04157e8af44e86cdbf28131afe265b1" ) );
        TEST_EQ( test_hex<shake_256>( 0, buf2 ), digest_from_hex( "ae55b62407b452c9488ac214076865348cc36488c8070b08528594aa50ffdb10a166887624a3070e5e6af739db85847c90545564c2dabae8e0282a7018600e9f0951be6c9be3d3a607428971b728af933b9c130ecc821a81063dbd1b7650ea27696890e6dfca4293a0a7d58f096fb5dc5aa0140517c8919d206fbd964be330fed2189f28beb719a8" ) );
        TEST_EQ( test_hex<shake_256>( 0, buf3 ), digest_from_hex( "1789bb9ead2c5443cba7ef660032d71e65259e6d303eed448706df02ad5cb01b132f260f212d810362e47c27402ce886e29fbd35c15373a0eb2d5004b55dccf1303e2443b7d1a04ad15bb2537f2bfc34288e5ca03c8c0c1991364c2773a0016c453bedd5b27ab1ceb5f51005312fc9185c093d7d88fb9a6f98f42176e685a052943ac14a8de4de85" ) );
        TEST_EQ( test_hex<shake_256>( 0, buf4 ), digest_from_hex( "b34aefbf50100fa619bd24555d00894d634fe17e15e2de0e7163538bd9b5c25694fed145c2aac0937e794cfe01289915b5a0b05b988c338dd5b5e227c121300f706ca35df134a3e1e2d8b46a38f00ed49fed416da077d7f8305d9953f07b5c36854d18c819ee937dcb82e31fe7c5237d82c5d35025e33b6090bbfb43a3e60d73487ee4a42f787c5a" ) );
        TEST_EQ( test_hex<shake_256>( 0, buf5 ), digest_from_hex( "e682ca4670360a7a2d10d97ed3d41a31831b4a2169d438c6515cc813f2afd266e52cba68772b4a1d83fde8046b31485d5f400e1de8d92da62d2c258f33efd6dbe621e5219d3ae4da6bd529d8800da6f26ba827d4fce56a014fd5e4e45d3362f8197d0d9ba5a287af2c7a31f41da4621c36d2ce5d92d837549bb434c0e1f99cb52b940bb6bc87da61" ) );
        TEST_EQ( test_hex<shake_256>( 0, buf6 ), digest_from_hex( "b8e3d7cc69d352a694e87e753218c54f50d9d68bc279ddfaaf727162716a8586a5916f8d67a6e78979a2d892520693cb3719c1f31aedb8f322f78a1370c852d9634c9eb7a738a7b670adc5c2eb3a94c516657bfa584bed8f0bf4bbc7311d4ef41ea549d818554e762c493d2750d55797f3f00047442d406ed8f0d2ad99e9c290e1ef15cd9e2b8f9b" ) );
        TEST_EQ( test_hex<shake_256>( 0, buf7 ), digest_from_hex( "cae6569da1f190633fe9984d65a7c2a4c0dfc173558c35b55e735ab0b51319bdc84324352428f0e17958ec321b3d955ebd4d1d4cf8e14bf4aabed3c458933a6c139840120ff8f994dc83ac0377947f996274792f5f7769b93db48933b177cf06b1d9188da2fed08a10501a99614e627beba1cde89da35d906c65bd6a3ecb2985b31cf44d32f5cfdf" ) );
        TEST_EQ( test_hex<shake_256>( 0, buf8 ), digest_from_hex( "31a343cf77cbcbf50e407fbbea061e5cd5d211dba76368795743f170a415139aad785873bf8b85cad3de5de65d8e949fd01d2be7b8db5882f8bbd1ca7dd0e00790e811770507bf528052b0515f984e85ba5b6edfa7efbdadde65ec48214694fbdf91a94e529b3a13d8213e33949b7f78ca7f2e8414acb0e6027b3101b51c036cf9a7276596fb8dcf" ) );

        TEST_EQ( test<shake_256>( 7, str1 ), digest_from_hex( "a98e0a6e0e0510cbde22f1bb953abdaec816ded1982f02dab9f7094c1ad56a62e2a1169ac6b540cee8c16d846245a9911cfba87a208f4317c46b9a2a6196bc401247c40eec30200480bd5afadccdd5007aa3d38315428ca704e72b045e2ceb60c8f0627e149396bfbcf25acb2e31b990f888b202bc4a48462e60df572feeafe75bc50ca468235774" ) );
        TEST_EQ( test<shake_256>( 7, str2 ), digest_from_hex( "3bafc7d2e320f1a1c3fa3de31322361a5df67347da3f3c4c4e739315de26b65d69a2c1a469851e3b9654355cb299908c2c8c5339d1e680a79de876898831042230d60c5a7b570a4e49774d7dcec6d4efc04b75eebbd6dc45bec25585220dbaecbe279d40a9e0604f9bfa6842348a192bb572faafd78ccb0a91e0afda9c7842ad8ccc2c08192865aa" ) );

        TEST_EQ( test_hex<shake_256>( 7, buf1 ), digest_from_hex( "94f3a7d3ac28e9c21e977115964633eafd70773e09f91befea976788db5fbb3740cbba45ac56c8da1d3917bdb8379310813e53d5be19ee7f25dfe236e401a78feeb97a99b9b530c9a53da6efbf955605df93fd83d18c42a2204e8f5ff877141ff7b541d1e2561ac3d8bb1ea0f0359865e6254a25dacbef740958a361c73baa635786bb242489268e" ) );
        TEST_EQ( test_hex<shake_256>( 7, buf2 ), digest_from_hex( "56391a867b9c78407b1c601ac39c768715dcd219b1cb0050ccc4b04dedd6e01f99ee35568dc6546ef217d3418ced300c9d552d18e2ed6a25fa03e85a2f624fed91d962c7d61065165ce39638e4121fbf0b5e9535de04cc3833d27c5fcf2162a754af71d7fb4b3911febe671126896393cc95273a03aa500caa2c1c3347b1a331acc4c818c679709e" ) );
        TEST_EQ( test_hex<shake_256>( 7, buf3 ), digest_from_hex( "e1f8764d4ca31a2073bb9dd9d13a249e5c0d350b7de33cf246e0654f2287a0cf26b5a1a0c02bdd52721174ff90e58c4e2603a8a4507b657715a88993154f0db07ae6e634e2364434f22de4b4378656877d024e6e9c0652ce1fb2057caf8343f85e339621c20cdf78172d22b9e56e8bf564fd97ff0f7234b37ff319857385c51d206173536d50ae20" ) );
        TEST_EQ( test_hex<shake_256>( 7, buf4 ), digest_from_hex( "0b3506107e9d6c7397b6acf16c2b3a27d038cfdcf4d87233638df48cec9d8142e42ade0b25ebc03a93be7c4be3012cd5b590cd30a11cb931656150db5ed48c062dd4f0e36fa823b5661c8ada968b27e64f7372ea8cd52a98291322229530f9394ecbbe34d5704c48c428e1edce5c1401061fe66911c9df43eefbc17258c7b6b7ea0809529ab3e652" ) );
        TEST_EQ( test_hex<shake_256>( 7, buf5 ), digest_from_hex( "a2d533f215c0237f0502a122b0803c881677ecc6cf106871511485d1793e0549cd90fa242bcc18cc7ca25cf358eb0d1761f7023bcd97f9a071e5ed6c0f8c72e4e727288b64af56345613cad98b860801fcd839e6004b101b10be74392ad1a4f7871808307d2a576a2c98712fc04117f1abff0684a318d35751922077e97f4ce09072cd4fa3b3dc87" ) );
        TEST_EQ( test_hex<shake_256>( 7, buf6 ), digest_from_hex( "fadc0fda0834ae705c7e985e7f523c4b998de801d1d9b21877ffa8eaa2b61ba36d2fbf6f5248b3f5140c6e183a6716dd05582573bfe6f6648ddbdfe8620feeedc7b546c501a04adccbccf8248e759a9ee35f11eff25c3dcbb530c56386f45f461b353139fa1e2561421f6820b11ef3704263a6b44530deac121b78d6bff2ff50d5be89da6c449cd9" ) );
        TEST_EQ( test_hex<shake_256>( 7, buf7 ), digest_from_hex( "16a85ba48664ce3f901585a65fb9111e2404c9d4ab1476301a64802c3410f628ea7b6bf7dfb7ed0be0072173734c3ed1d5cb1b891f6a33812eb41c9e3f2dcae700937485cb0dd0c21183e5ba131206e75da0ee25a43f3d47bbdf9e2ece96d2b3d9cc181f8a45bf21d42fafa7994bba607f0de91669c90f63878169a0aa961ab147371186109a35fa" ) );
        TEST_EQ( test_hex<shake_256>( 7, buf8 ), digest_from_hex( "9316dc6084c5e9b290de57fd04389675a3fdee82ba78a977658a411f8f6e13ef1e83e38a26d2f98d5823e8b8766af8a15fe8aa4bc26b0d19fa53ce41d292a0d259e31b5d787773d6b90ed8afdef4e6ae03643e26c9526a495f971da50cafba2140633769b60c8acbc3ff3d8d37976450c6dcef31c1f2075cc5d34c4d85e6f330feab105e9721d37f" ) );
    }

    return boost::report_errors();
}
