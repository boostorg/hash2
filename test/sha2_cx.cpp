// Copyright 2024 Christian Mazakas
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/hash2/sha2.hpp>
#include <boost/hash2/digest.hpp>
#include <boost/hash2/detail/config.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/config.hpp>

#if defined(BOOST_MSVC) && BOOST_MSVC < 1920
# pragma warning(disable: 4307) // integral constant overflow
#endif

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

#if defined(BOOST_NO_CXX14_CONSTEXPR)
# define TEST_EQ(x1, x2) BOOST_TEST_EQ(x1, x2)
#else
# define TEST_EQ(x1, x2) BOOST_TEST_EQ(x1, x2); STATIC_ASSERT(x1 == x2)
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
        constexpr char const str2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
        constexpr char const str3[] = "\xbd";
        constexpr char const str4[] = "\xc9\x8c\x8e\x55";

        TEST_EQ( test<sha2_256>( 0, str1 ), digest_from_hex( "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" ) );
        TEST_EQ( test<sha2_256>( 0, str2 ), digest_from_hex( "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1" ) );
        TEST_EQ( test<sha2_256>( 0, str3 ), digest_from_hex( "68325720aabd7c82f30f554b313d0570c95accbb7dc4b5aae11204c08ffe732b" ) );
        TEST_EQ( test<sha2_256>( 0, str4 ), digest_from_hex( "7abc22c0ae5af26ce93dbb94433a0e0b2e119d014f8e7f65bd56c61ccccd9504" ) );

        TEST_EQ( test<sha2_256>( 7, str1 ), digest_from_hex( "9d2854465b76578f101c74586ff54adee8802ff646f3a26068669b50f1ebb2f4" ) );
        TEST_EQ( test<sha2_256>( 7, str2 ), digest_from_hex( "92a5bca8db752adf4303670c4707d0a6875d4ae9c05e68e9abda2866572497eb" ) );
        TEST_EQ( test<sha2_256>( 7, str3 ), digest_from_hex( "b4323e8acc99549bcf04d4d4fe68078b4cd7738c6b6d4d9648b7de78d72476e2" ) );
        TEST_EQ( test<sha2_256>( 7, str4 ), digest_from_hex( "9289493b29a4d51c8dda571c5d1332148255e7d1f5a77429c8664e41d7b4be37" ) );
    }

    {
        constexpr char const str1[] = "abc";
        constexpr char const str2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
        constexpr char const str3[] = "\xff";
        constexpr char const str4[] = "\xe5\xe0\x99\x24";

        TEST_EQ( test<sha2_224>( 0, str1 ), digest_from_hex( "23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7" ) );
        TEST_EQ( test<sha2_224>( 0, str2 ), digest_from_hex( "75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525" ) );
        TEST_EQ( test<sha2_224>( 0, str3 ), digest_from_hex( "e33f9d75e6ae1369dbabf81b96b4591ae46bba30b591a6b6c62542b5" ) );
        TEST_EQ( test<sha2_224>( 0, str4 ), digest_from_hex( "fd19e74690d291467ce59f077df311638f1c3a46e510d0e49a67062d" ) );

        TEST_EQ( test<sha2_224>( 7, str1 ), digest_from_hex( "8d80f3b3d22f69db114c0bf5c0236e404789d34622f52ea50f3a4a44" ) );
        TEST_EQ( test<sha2_224>( 7, str2 ), digest_from_hex( "1d8ad5f9f2a8a2c868dc7b11c63355c52c08038ae95e3c3b0e36ab19" ) );
        TEST_EQ( test<sha2_224>( 7, str3 ), digest_from_hex( "33f07b651d2ec15edb7a7c674dafb1aa4fba63de6b9dfd53c96efca5" ) );
        TEST_EQ( test<sha2_224>( 7, str4 ), digest_from_hex( "2c8caaa814b5974f1c005db9dbae590d2b478f1e6f23c431b0d58556" ) );
    }

    {
        constexpr char const str1[] = "abc";
        constexpr char const str2[] = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

        // https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/SHA2_Additional.pdf
        constexpr unsigned char buf1[ 111 ] = {};
        constexpr unsigned char buf2[ 112 ] = {};
        constexpr unsigned char buf3[ 113 ] = {};
        constexpr unsigned char buf4[ 122 ] = {};
        constexpr unsigned char buf5[ 1000 ] = {};

        TEST_EQ( test<sha2_512>( 0, str1 ), digest_from_hex( "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f" ) );
        TEST_EQ( test<sha2_512>( 0, str2 ), digest_from_hex( "8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909" ) );
        TEST_EQ( test<sha2_512>( 0, buf1 ), digest_from_hex( "77ddd3a542e530fd047b8977c657ba6ce72f1492e360b2b2212cd264e75ec03882e4ff0525517ab4207d14c70c2259ba88d4d335ee0e7e20543d22102ab1788c" ) );
        TEST_EQ( test<sha2_512>( 0, buf2 ), digest_from_hex( "2be2e788c8a8adeaa9c89a7f78904cacea6e39297d75e0573a73c756234534d6627ab4156b48a6657b29ab8beb73334040ad39ead81446bb09c70704ec707952" ) );
        TEST_EQ( test<sha2_512>( 0, buf3 ), digest_from_hex( "0e67910bcf0f9ccde5464c63b9c850a12a759227d16b040d98986d54253f9f34322318e56b8feb86c5fb2270ed87f31252f7f68493ee759743909bd75e4bb544" ) );
        TEST_EQ( test<sha2_512>( 0, buf4 ), digest_from_hex( "4f3f095d015be4a7a7cc0b8c04da4aa09e74351e3a97651f744c23716ebd9b3e822e5077a01baa5cc0ed45b9249e88ab343d4333539df21ed229da6f4a514e0f" ) );
        TEST_EQ( test<sha2_512>( 0, buf5 ), digest_from_hex( "ca3dff61bb23477aa6087b27508264a6f9126ee3a004f53cb8db942ed345f2f2d229b4b59c859220a1cf1913f34248e3803bab650e849a3d9a709edc09ae4a76" ) );

        TEST_EQ( test<sha2_512>( 7, str1 ), digest_from_hex( "99db140aa8d0d208a5ceae58803a56b79f0e5143beb8d6914ace1494ce97a6b1cb16326ca71b2c3d61bfe82812206a14d272a913f1631d950fc66e9b33bd1121" ) );
        TEST_EQ( test<sha2_512>( 7, str2 ), digest_from_hex( "5f2c662e3345a4e4cb9a0e2b223b96c93a89d1f562e15b87ee042b7dba14b7335b5260d9354dbb3f0ccdce6a58c15b0fec18d3634cbb64159c25c05efa967359" ) );
        TEST_EQ( test<sha2_512>( 7, buf1 ), digest_from_hex( "7089655eee37759b4fe9925fc6da3ff3be9b66bddd944e13782e695670b75405bc63d4cdacaa5cdc2629d80b5500d9fbdd6e5790374edfcd618ff9ea2cc067c2" ) );
        TEST_EQ( test<sha2_512>( 7, buf2 ), digest_from_hex( "6f769c062eaec5dfe83ca575df62257002c8528a2e11eebb65d1a0efa8b314dcfce9f82d2ad1677c4cc87918c0741296388b27bf5727025e6be3be93d2e09248" ) );
        TEST_EQ( test<sha2_512>( 7, buf3 ), digest_from_hex( "8d185b313bd4c1bc0641ed83a382bdb00b96b30f5d8bfab5d9546963a18effe19e9bdd67d763eaebe93994d348dd968dc673ff4eecf559ae0ecfdea7ea8b955f" ) );
        TEST_EQ( test<sha2_512>( 7, buf4 ), digest_from_hex( "73db5cf67f0b0f61e160dcd9e983d61c65b18a2d8ae6775af62005fdcc3059c488bfedf57ed79c2ee0d0779695c5e747cb76be35f7647b9da49541e4f9a219d1" ) );
        TEST_EQ( test<sha2_512>( 7, buf5 ), digest_from_hex( "922d567e5ade55ac5a16bfb0988eabfc3637b4f930cfb2a548cfb0c3fdcafc4f3333f1f14fcec6d0c9638220a6bc1c81c926052a51aa0305c8a7cfc43693fce9" ) );
    }

    {
        constexpr char const str1[] = "abc";
        constexpr char const str2[] = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

        // https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/SHA2_Additional.pdf
        constexpr unsigned char buf1[ 111 ] = {};
        constexpr unsigned char buf2[ 112 ] = {};
        constexpr unsigned char buf3[ 113 ] = {};
        constexpr unsigned char buf4[ 122 ] = {};
        constexpr unsigned char buf5[ 1000 ] = {};

        TEST_EQ( test<sha2_384>( 0, str1 ), digest_from_hex( "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7" ) );
        TEST_EQ( test<sha2_384>( 0, str2 ), digest_from_hex( "09330c33f71147e83d192fc782cd1b4753111b173b3b05d22fa08086e3b0f712fcc7c71a557e2db966c3e9fa91746039" ) );
        TEST_EQ( test<sha2_384>( 0, buf1 ), digest_from_hex( "435770712c611be7293a66dd0dc8d1450dc7ff7337bfe115bf058ef2eb9bed09cee85c26963a5bcc0905dc2df7cc6a76" ) );
        TEST_EQ( test<sha2_384>( 0, buf2 ), digest_from_hex( "3e0cbf3aee0e3aa70415beae1bd12dd7db821efa446440f12132edffce76f635e53526a111491e75ee8e27b9700eec20" ) );
        TEST_EQ( test<sha2_384>( 0, buf3 ), digest_from_hex( "6be9af2cf3cd5dd12c8d9399ec2b34e66034fbd699d4e0221d39074172a380656089caafe8f39963f94cc7c0a07e3d21" ) );
        TEST_EQ( test<sha2_384>( 0, buf4 ), digest_from_hex( "12a72ae4972776b0db7d73d160a15ef0d19645ec96c7f816411ab780c794aa496a22909d941fe671ed3f3caee900bdd5" ) );
        TEST_EQ( test<sha2_384>( 0, buf5 ), digest_from_hex( "aae017d4ae5b6346dd60a19d52130fb55194b6327dd40b89c11efc8222292de81e1a23c9b59f9f58b7f6ad463fa108ca" ) );

        TEST_EQ( test<sha2_384>( 7, str1 ), digest_from_hex( "3120f025f5fefd4d12d86c805550c2d825b7071c6d6f8c9a0ac514285bb86f1715e1486ecf1938f9d38e774a51306f93" ) );
        TEST_EQ( test<sha2_384>( 7, str2 ), digest_from_hex( "7581026b888b43b55c63b3ef0e422a6f727e7b4fc8e24a397842e60899e11cd31f4532a9b54762c236b049cfd907b35d" ) );
        TEST_EQ( test<sha2_384>( 7, buf1 ), digest_from_hex( "a88573a36ffba8d86d0bcbc99af7d95b2b2232dd45e499fa965cc68fdc3564c781e3c44d4c02a1919587de12b3286ed5" ) );
        TEST_EQ( test<sha2_384>( 7, buf2 ), digest_from_hex( "3e04053357c78210321e9ac456b944b3687c2cf042fa601911c3b3b112444db0cce8f9e8de7f339ac88b03566e29161b" ) );
        TEST_EQ( test<sha2_384>( 7, buf3 ), digest_from_hex( "a981a2372cc77eae0581fbee3844e6587cd0fa70b88b9c7a9d17e344bac3ab72aff0631af056d35ec34911f265ddb651" ) );
        TEST_EQ( test<sha2_384>( 7, buf4 ), digest_from_hex( "252d43fa5c979cabc59893cee91666b06605daac59d84dc1fcfb6833a9f439a1e4616662cdfdc6b664af49f7b177de8b" ) );
        TEST_EQ( test<sha2_384>( 7, buf5 ), digest_from_hex( "96e4c708bcf9ba9c8fb37ec05ae9d5fb09e69894eb51d0e2a638db3a74c500415c841f732bde72a0cd63780a9b7e062b" ) );
    }

    {
        constexpr char const str1[] = "abc";
        constexpr char const str2[] = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

        constexpr char const buf1[] = "cf";
        constexpr char const buf2[] = "ca2d";
        constexpr char const buf3[] = "497604";
        constexpr char const buf4[] = "3c7e038401fa74c6c06e41";
        constexpr char const buf5[] = "40bd7d47b636c2a749a247fdda75807c238b";
        constexpr char const buf6[] = "cd5fee5fde5e9aa2884b4f4882cfa7d5571f8fd572c5f9bf77a3d21fda35";
        constexpr char const buf7[] = "f15284a11c61e129ea0606bd6531f2f1213776e01e253d1def530bed1c3c42b3c68caa";
        constexpr char const buf8[] = "e3089c05ce1549c47a97785d82474c73cb096a27c5205de8ed9e3a8c971f7fa0eab741fd2c29879c40";

        TEST_EQ( test<sha2_512_224>( 0, str1 ), digest_from_hex( "4634270f707b6a54daae7530460842e20e37ed265ceee9a43e8924aa" ) );
        TEST_EQ( test<sha2_512_224>( 0, str2 ), digest_from_hex( "23fec5bb94d60b23308192640b0c453335d664734fe40e7268674af9" ) );

        TEST_EQ( test_hex<sha2_512_224>( 0, buf1 ), digest_from_hex( "4199239e87d47b6feda016802bf367fb6e8b5655eff6225cb2668f4a" ) );
        TEST_EQ( test_hex<sha2_512_224>( 0, buf2 ), digest_from_hex( "392b99b593b85e147f031986c2a9edfdb4ffd9f24c77c452d339c9fc" ) );
        TEST_EQ( test_hex<sha2_512_224>( 0, buf3 ), digest_from_hex( "a9c345d58a959af20a42c84e28523ba47e3bf8fad8e8c3f32b7a72ae" ) );
        TEST_EQ( test_hex<sha2_512_224>( 0, buf4 ), digest_from_hex( "a74af68819afe81bcdaceba64201c0d41f843e4b08e4002a375be761" ) );
        TEST_EQ( test_hex<sha2_512_224>( 0, buf5 ), digest_from_hex( "087ed68f1db90ffb2fb4ff7dc4b17fe08100b64383850378ef543339" ) );
        TEST_EQ( test_hex<sha2_512_224>( 0, buf6 ), digest_from_hex( "72b43417b071f4811833027731b0ca28549c0357530fe258ca00533e" ) );
        TEST_EQ( test_hex<sha2_512_224>( 0, buf7 ), digest_from_hex( "fa425bc732d6033566c073560b2c5fe322aa4fa22aaa3ec51154ffd8" ) );
        TEST_EQ( test_hex<sha2_512_224>( 0, buf8 ), digest_from_hex( "75a0fa978c45d268124d8cd9ef0a08ecabbbed53412cfc7cb1c00398" ) );

        TEST_EQ( test<sha2_512_224>( 7, str1 ), digest_from_hex( "d3e6a9cbb2486aa93f16134cb4b4ab8d91bcba5fb1d4d2c5b1847f21" ) );
        TEST_EQ( test<sha2_512_224>( 7, str2 ), digest_from_hex( "e5d89e509f480d0174364a101fd7a9cefed2e0a18db0987858aac52c" ) );

        TEST_EQ( test_hex<sha2_512_224>( 7, buf1 ), digest_from_hex( "8c187000d2d05c42f693552bbc021cbc2aa62813ea45694bcf32ca96" ) );
        TEST_EQ( test_hex<sha2_512_224>( 7, buf2 ), digest_from_hex( "29fefffc33fc42baedb63cd2664985af132c64687e3f21a9793377e7" ) );
        TEST_EQ( test_hex<sha2_512_224>( 7, buf3 ), digest_from_hex( "e71cb2f9ddd2c0994c8a24e547a8b0f0244d75bdb9812d3e18f3d6ae" ) );
        TEST_EQ( test_hex<sha2_512_224>( 7, buf4 ), digest_from_hex( "a235fd5e1b0715283cb872ca713c802734142df549ffaea84dbf2949" ) );
        TEST_EQ( test_hex<sha2_512_224>( 7, buf5 ), digest_from_hex( "c58912d0fd8cdf7304ee901565024b0e544c0c5a96c2724f287de580" ) );
        TEST_EQ( test_hex<sha2_512_224>( 7, buf6 ), digest_from_hex( "1d58df4e8e8bc6ea6ed2de7125e177a1fc474fd1fb457457696681c7" ) );
        TEST_EQ( test_hex<sha2_512_224>( 7, buf7 ), digest_from_hex( "7ee4d395d55044d3e6c6ec00f6991d1541820f36421c2762c25611bf" ) );
        TEST_EQ( test_hex<sha2_512_224>( 7, buf8 ), digest_from_hex( "21acc1056aa8af3f8f766e29f53dc1bd9f77806d22e3dc84875c32ac" ) );
    }

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

        TEST_EQ( test<sha2_512_256>( 0, str1 ), digest_from_hex( "53048e2681941ef99b2e29b76b4c7dabe4c2d0c634fc6d46e0e2f13107e7af23" ) );
        TEST_EQ( test<sha2_512_256>( 0, str2 ), digest_from_hex( "3928e184fb8690f840da3988121d31be65cb9d3ef83ee6146feac861e19b563a" ) );

        TEST_EQ( test_hex<sha2_512_256>( 0, buf1 ), digest_from_hex( "c4ef36923c64e51e875720e550298a5ab8a3f2f875b1e1a4c9b95babf7344fef" ) );
        TEST_EQ( test_hex<sha2_512_256>( 0, buf2 ), digest_from_hex( "0c994228b8d3bd5ea5b5259157a9bba7a193118ad22817e6fbed2df1a32a4148" ) );
        TEST_EQ( test_hex<sha2_512_256>( 0, buf3 ), digest_from_hex( "a9e2427cec314b2814aaba87039485fc8d3ade992fa1d9acbb7f6769460a7317" ) );
        TEST_EQ( test_hex<sha2_512_256>( 0, buf4 ), digest_from_hex( "447044f03bc30e2caa245d26ce4c72c1454f708cfcd9a215841a88cf5ecd2095" ) );
        TEST_EQ( test_hex<sha2_512_256>( 0, buf5 ), digest_from_hex( "5424b31989031809b5fc8969b7c48b5dc0c233fc34ffb5b223cd5f3a9712a8d6" ) );
        TEST_EQ( test_hex<sha2_512_256>( 0, buf6 ), digest_from_hex( "41c7027d16e37259645d0173c86141f38d808e9e27dc2dfeeaf335ed7c99490c" ) );
        TEST_EQ( test_hex<sha2_512_256>( 0, buf7 ), digest_from_hex( "683b486861e598dabba740ac919522cf3b609c18205b6beca4ccbe6b0f6dc6db" ) );
        TEST_EQ( test_hex<sha2_512_256>( 0, buf8 ), digest_from_hex( "035d362a4265031434c3aa153ab4aef6b00d5176227197430bb62884a085c5fe" ) );

        TEST_EQ( test<sha2_512_256>( 7, str1 ), digest_from_hex( "e176ea40b8effa484174ecbd3d81263bc57f995ffa20016725bbefba2a042f0a" ) );
        TEST_EQ( test<sha2_512_256>( 7, str2 ), digest_from_hex( "07c475910244c2f66521385a836aab7132bb207971e043a00859d3ed589c0738" ) );

        TEST_EQ( test_hex<sha2_512_256>( 7, buf1 ), digest_from_hex( "59bb77ffd9ad2f380f15b8b8de5493f28f2c52df55bae55bb5396583d93fbc05" ) );
        TEST_EQ( test_hex<sha2_512_256>( 7, buf2 ), digest_from_hex( "e6e96f3f396ddc56527c42c4b5bd1b2892ca9b95ac3d877fd9df9a2a42b942a7" ) );
        TEST_EQ( test_hex<sha2_512_256>( 7, buf3 ), digest_from_hex( "330d4160f591727b1d7f0f03ff983942d6146326e5f20484a97d381e19c9ea06" ) );
        TEST_EQ( test_hex<sha2_512_256>( 7, buf4 ), digest_from_hex( "7559ad3587639d7bce4534f67560f541676762b8ad1b9eda289fbe128781a2e8" ) );
        TEST_EQ( test_hex<sha2_512_256>( 7, buf5 ), digest_from_hex( "b77c34dbfb7e15b7573d08778a138842a600e11ac90e5e2919b7c267b4a35638" ) );
        TEST_EQ( test_hex<sha2_512_256>( 7, buf6 ), digest_from_hex( "d025d6fb188587173c4f35878711c7daded4c7c899c780dddff8922914a416bf" ) );
        TEST_EQ( test_hex<sha2_512_256>( 7, buf7 ), digest_from_hex( "3281346a8282df6a78077f03fec6a97daa575cab6488743396782b20fda75b95" ) );
        TEST_EQ( test_hex<sha2_512_256>( 7, buf8 ), digest_from_hex( "5ac9c2acac13ba706eb192adb0923c4d530bbd1e85389485f16343c188078dfb" ) );
    }

    return boost::report_errors();
}
