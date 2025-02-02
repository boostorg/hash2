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

        TEST_EQ( test<shake_256>( 7, str1 ), digest_from_hex( "e40fa8d3828ec73d85202f3dc09b1b8eb38e36e19a7adfc9734c26732a21bccddde202c19a5f07e4a404a5964fcb66d871a411a8a38fbb69fb68143d9d5a011d75eacc924043f105e18fe60bf7a98d1e6658c0f725cb143f58a2ada2d691292f64b83c5e146c13027e87a743a1ebc92622ff5be73b9a3957e21756d1b7110f10533cc7a3cdff8db6" ) );
        TEST_EQ( test<shake_256>( 7, str2 ), digest_from_hex( "a431afef981a49feb1f05319417803eb89f8ab0a722396bab806629445fe0c145827c0d5b6fb71998144fa848c6680133b72cbf9c194bc0a59a8b2ec26d96c1e14cb553aa1ebe9fd13f77645f8a2bebe21f7d114220ed19dcf9b951a1cd6bc67db0619871eed99d779c41275cb99bff36341c165bb6f1c9cec1a40c52ccf2162c9c3ba9528d74ec5" ) );

        TEST_EQ( test_hex<shake_256>( 7, buf1 ), digest_from_hex( "0f57c9a576901e66efa54138cadf3f989fc0eb7725d8794d7dbc6741d26d1244190aad0d4aca76541fde991700619237703ee6ef511d513ddde7837cfef8a4e66adab0a8e5215452d98111ff571977e45f4220b87fab6477d7dc36ce6c2b2ac8b169e84e094bc84191ea74422a6179a42fb4e67ba430d9e38e5d11a909cdb056f92eeb88827632b3" ) );
        TEST_EQ( test_hex<shake_256>( 7, buf2 ), digest_from_hex( "02145701fbeadf814631081fb0f78ba72c7dac88420e5f68b2dcfba16420ba8631200f1e757e3dc01c40c63fca60e72860db8f532ee545c490e15034deabf87439fac04db189a448b8b0717473a4f4d57bcf95e028fd1e7a91b7558e7c0b2465f130aad20cc97ee5edfcf32f5d32d41d9aa8ea1677b9b7ccb7e06643c84c11da519d61affea36216" ) );
        TEST_EQ( test_hex<shake_256>( 7, buf3 ), digest_from_hex( "cacd54e2dfcbd72f603fdc40efa6c471770fb3efff4e043587523dc06f9b9bc91de2c4d218d53e3c08caedd8f05929a77d959e04a2825db23c59e033336d29fce5b9e9c1131523cfe7321897bd88b01802ff60924f6bba69229ff888fa464604cff1957e7221193234ca7a2d82073b56e7a3a4a39baa323a02b530f59302f5325a3f746b55ebdf5c" ) );
        TEST_EQ( test_hex<shake_256>( 7, buf4 ), digest_from_hex( "b79813ddda6802fa2e8e771b2161e579614e3e85e04c0ab77b5dc09f166b900551b664012a3fa6976a948201d019a44a97f6c6f6fd527973fc06fdd6c3be63722f70fc77ae2f27f880b5538635f30272b2e621513987f7e7a339a8c72105d265b79a014cd089f60e62e5e18d197e897133a3da056a29b28503c9036734e4a2557989bb0705cf649e" ) );
        TEST_EQ( test_hex<shake_256>( 7, buf5 ), digest_from_hex( "fcac6ceb84533e0d5377e4567bbe2d01eb1594bab0855caafe2d14e3ca24c9eb9f22771231ec9f224d515e8f540ddea2acd982dd5f4e1157c40b72905c136edf0d12df345cfcd88a740ba85484801fd52a08ad768b0eca1405a02ae71b1b8f81c81c3c9bdf76ad97383b4a65a25f8881d5623b04af0c882996b4c840553b35a7edbd3928c61f4974" ) );
        TEST_EQ( test_hex<shake_256>( 7, buf6 ), digest_from_hex( "1be4db66fbb9b300ed6ebf4ed70ef74f84ecb0c69a18ef981a36b263d06dbcb2f062618a0845a8d96352052d1cc10854d4be87b24f7bc8eb6e05a7711cafa2543555f611dd54703b1dfdca9ca98b509eac77d7f51dc6f094c02d2c00e8816d880ff28ce616559f65de1c96409fa06c4b7b17a5fb36ccd0d5259aa271918126d9382ec58c992fa92f" ) );
        TEST_EQ( test_hex<shake_256>( 7, buf7 ), digest_from_hex( "0e2e86663f92dfd5717a691ed547136406f6710a9f8fc54982165fc0915960e072b5c3d3ed55dc867362b5339b7fa951940f66bf593249898c6f8c2bbde77a5cd8b2e15cd1c5027aa4d1c17e5ea746fd65d9e9b4c41b15d30019c7bf8743ed52d70ee8d80c2b492c0c20aea167d534260be9f18bcdae5d9f5da17bcee5a0b3275eb47df754297a42" ) );
        TEST_EQ( test_hex<shake_256>( 7, buf8 ), digest_from_hex( "300655897163ea650f76e68dc35ae760a03109d7ab4f8ecafacde42171df47c919fa53db627dc8fa09b7e22b3b45313c732c689b1ebb6449e22f9e10d74f09da0e39f6ab0a4be447e309a2d6033c5204e3a5c3711e4b22664f3b06c3c8fb86ef459491fc11d11110b2f5fd716f1092e80b0a7021d1439fb4831e0a8ce8673dac844e602f3f88e1ca" ) );
    }

    return boost::report_errors();
}
