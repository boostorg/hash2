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

// msvc-14.2 will OOM
// gcc-5 has issues with these constexpr tests:
// libs/hash2/test/sha3_cx.cpp:100:132: internal compiler error: in fold_binary_loc, at fold-const.c:9925
//          TEST_EQ( test<sha3_256>( 0, str1 ), digest_from_hex( "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532" ) );
#if defined(BOOST_NO_CXX14_CONSTEXPR) || BOOST_WORKAROUND(BOOST_GCC, >= 50000 && BOOST_GCC < 60000) || BOOST_WORKAROUND(BOOST_MSVC, >= 1920 && BOOST_MSVC < 1930)
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
        constexpr char const str2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
        constexpr char const str3[] = "\xbd";
        constexpr char const str4[] = "\xc9\x8c\x8e\x55";

        TEST_EQ( test<sha3_256>( 0, str1 ), digest_from_hex( "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532" ) );
        TEST_EQ( test<sha3_256>( 0, str2 ), digest_from_hex( "41c0dba2a9d6240849100376a8235e2c82e1b9998a999e21db32dd97496d3376" ) );
        TEST_EQ( test<sha3_256>( 0, str3 ), digest_from_hex( "b389fa0f45f21196cc2736e8de396497a2414be31e7a500a499918b8cf3257b2" ) );
        TEST_EQ( test<sha3_256>( 0, str4 ), digest_from_hex( "faad1cd75a6947e88d210123959b29d8f0973b8d094582debe56742878f412f6" ) );

        TEST_EQ( test<sha3_256>( 7, str1 ), digest_from_hex( "332937c247b4e748e1fc9edc7ee984c3b4a7726fcc5a844e8553c5ee87c2e43d" ) );
        TEST_EQ( test<sha3_256>( 7, str2 ), digest_from_hex( "b6175b25d5dd7839110e380aa21aa15f0c9100671510da570897e40982d4b369" ) );
        TEST_EQ( test<sha3_256>( 7, str3 ), digest_from_hex( "3eeeeac60b652f3bcffdf2fc833bf1cba6f703a5d07bc9eac395e5526f0818db" ) );
        TEST_EQ( test<sha3_256>( 7, str4 ), digest_from_hex( "2b368666aed0d0de497b88a09345d74099c4e83632c8bc8b839db98760ff053f" ) );
    }

    {
        constexpr char const str1[] = "abc";
        constexpr char const str2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
        constexpr char const str3[] = "\xff";
        constexpr char const str4[] = "\xe5\xe0\x99\x24";

        TEST_EQ( test<sha3_224>( 0, str1 ), digest_from_hex( "e642824c3f8cf24ad09234ee7d3c766fc9a3a5168d0c94ad73b46fdf" ) );
        TEST_EQ( test<sha3_224>( 0, str2 ), digest_from_hex( "8a24108b154ada21c9fd5574494479ba5c7e7ab76ef264ead0fcce33" ) );
        TEST_EQ( test<sha3_224>( 0, str3 ), digest_from_hex( "624edc8a3c0c9d42bc224f0bf37040483432d7a1aeb68935e80f1e1c" ) );
        TEST_EQ( test<sha3_224>( 0, str4 ), digest_from_hex( "a2af03379fbe66d6a16b4235f124516bea38e5e5a83faa0d7175b7b7" ) );

        TEST_EQ( test<sha3_224>( 7, str1 ), digest_from_hex( "59ba0702bb53175bcc14d33163d2750282e4a2c8edbf3cc5d45f749e" ) );
        TEST_EQ( test<sha3_224>( 7, str2 ), digest_from_hex( "7f105336834af8cfebfb2b0ae74b74efc7cf44b3dd6e1b6d79020007" ) );
        TEST_EQ( test<sha3_224>( 7, str3 ), digest_from_hex( "908f10033ebc500ba5b5454b830816e1c4517ea9cacac9ce418ebe08" ) );
        TEST_EQ( test<sha3_224>( 7, str4 ), digest_from_hex( "1d0b1e02de8c23ab246eab2539ec90d02824e6912cf595c8d8ae5996" ) );
    }

    {
        constexpr char const str1[] = "abc";
        constexpr char const str2[] = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
        constexpr unsigned char buf1[ 111 ] = {};
        constexpr unsigned char buf2[ 112 ] = {};
        constexpr unsigned char buf3[ 113 ] = {};
        constexpr unsigned char buf4[ 122 ] = {};
        constexpr unsigned char buf5[ 1000 ] = {};

        TEST_EQ( test<sha3_512>( 0, str1 ), digest_from_hex( "b751850b1a57168a5693cd924b6b096e08f621827444f70d884f5d0240d2712e10e116e9192af3c91a7ec57647e3934057340b4cf408d5a56592f8274eec53f0" ) );
        TEST_EQ( test<sha3_512>( 0, str2 ), digest_from_hex( "afebb2ef542e6579c50cad06d2e578f9f8dd6881d7dc824d26360feebf18a4fa73e3261122948efcfd492e74e82e2189ed0fb440d187f382270cb455f21dd185" ) );
        TEST_EQ( test<sha3_512>( 0, buf1 ), digest_from_hex( "c677adbab492fd76bff50e41bddae49b9c8fe0b47801a73a47632d8e895ccdc631e7f00f7f87fa5baec2ab86e47ea07072c829bdfe36f251e57d7f5507ff9d06" ) );
        TEST_EQ( test<sha3_512>( 0, buf2 ), digest_from_hex( "af8bd43e6f05532448f6509151a871db4ddd867f386aadb2b553a75e30f81bd3c2c79eec5cde15b0ef9be399ab166f4e1f2620934e584223d9f9504641a10f24" ) );
        TEST_EQ( test<sha3_512>( 0, buf3 ), digest_from_hex( "d501c1503b8ef0846ef4fef2a3383a98957296879a6047f3c9adea670666bdf8ba1c46283d99433bafab4df59f16da32fd66f1a43f7f4664b696635c92333e2c" ) );
        TEST_EQ( test<sha3_512>( 0, buf4 ), digest_from_hex( "017d18ab7042eacc42bcf9f100a2ba45a14594434e76b3e2c4c59bb7710b13e99d6388d2542d7f0318bb8e0467fe46f8a1277c6d26db59f09f28e46fa8b60815" ) );
        TEST_EQ( test<sha3_512>( 0, buf5 ), digest_from_hex( "422d49f19849352a41f4a61796feb28c18a79883fdb2cc52df63dc7cf0b13efddd757fac95861403b1cca3f28cb164f897df9bbfc2f579567d10f8fb21b5c6bf" ) );

        TEST_EQ( test<sha3_512>( 7, str1 ), digest_from_hex( "eda4819a1d3ebaebdbd62dc1beb8db71230fb61c266b65f2a6f090d0d8aa45ad61ce906d75ac814b45bae4023246fcfac16cd46fd28e798b6c2599bcae6c924b" ) );
        TEST_EQ( test<sha3_512>( 7, str2 ), digest_from_hex( "e348824e6fc0c3c89208841431ca501dc1c36b7ce10324a67e7bee930278b9c200a5c281e166c0751fa1852ee19700bc163f1e63a51b9125236e0a415dae44f3" ) );
        TEST_EQ( test<sha3_512>( 7, buf1 ), digest_from_hex( "124a2aae5e647bbd281a321032dcb21e76c0e45092bfbf798ff9bf17b963d4c189fcdd41bf59f7d19a509655cbc7974b72b97e99984100ef2c4042d952dc83af" ) );
        TEST_EQ( test<sha3_512>( 7, buf2 ), digest_from_hex( "c6dfb78a92d63a82fb57668a70ea480722879b06f310bcf04a00b771c92335a1461ee02ca94083e1405f92319f6206438c7e6ef1c2a28f6eabd2f4cfd9a12100" ) );
        TEST_EQ( test<sha3_512>( 7, buf3 ), digest_from_hex( "b5db27e2df1a9960d94811d1a2c79fb472a5d864edbc0312b6119fb63ec082be05dcbd1a66872b8049c5b34c0636242e1c29a4e3f625638682a6de31234a5df0" ) );
        TEST_EQ( test<sha3_512>( 7, buf4 ), digest_from_hex( "8b3d6c5596b989756b3c1339a5c4cac78a28c2cfc3913b635b63df77fac08a021d9cf6e9361d42511c568ecd8d14803a612b2c3f5937dc0e19bf566a480217d1" ) );
        TEST_EQ( test<sha3_512>( 7, buf5 ), digest_from_hex( "67548af9643ce72d83339b30fd18f50c2f1ca4806ce255eca7c314b7d7ad71dbb64bc0b162c79dba4a4b4643959dd06b0f34b23e6b931f778611bcbcfce86f44" ) );
    }

    {
        constexpr char const str1[] = "abc";
        constexpr char const str2[] = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
        constexpr unsigned char buf1[ 111 ] = {};
        constexpr unsigned char buf2[ 112 ] = {};
        constexpr unsigned char buf3[ 113 ] = {};
        constexpr unsigned char buf4[ 122 ] = {};
        constexpr unsigned char buf5[ 1000 ] = {};

        TEST_EQ( test<sha3_384>( 0, str1 ), digest_from_hex( "ec01498288516fc926459f58e2c6ad8df9b473cb0fc08c2596da7cf0e49be4b298d88cea927ac7f539f1edf228376d25" ) );
        TEST_EQ( test<sha3_384>( 0, str2 ), digest_from_hex( "79407d3b5916b59c3e30b09822974791c313fb9ecc849e406f23592d04f625dc8c709b98b43b3852b337216179aa7fc7" ) );
        TEST_EQ( test<sha3_384>( 0, buf1 ), digest_from_hex( "76578d4cc96f5abaa7b7676d75685eb58e3a9bae3574f5d4f208d7d16cfd251e34ef1cc673a3ee21c1511a1fedf4902b" ) );
        TEST_EQ( test<sha3_384>( 0, buf2 ), digest_from_hex( "5553d3b7829478df436c8f1c990144627413b2dc7da0a19e019ab249349f57dd29873c82b04066252a76933cf4c20c32" ) );
        TEST_EQ( test<sha3_384>( 0, buf3 ), digest_from_hex( "069e27853643a04ee85f1515f811e8b77697a78d155fb2c6c65cf1d86d84eb8c6af4639505f98c72d1ebdfa6da8fddbe" ) );
        TEST_EQ( test<sha3_384>( 0, buf4 ), digest_from_hex( "cd6131e2e00a6961663469d371de6f653c3df25e792575461ba5add1bbb0669410fff5e0b8e39e24af91821cbb2fb4e6" ) );
        TEST_EQ( test<sha3_384>( 0, buf5 ), digest_from_hex( "32bcf4f306fadc00701becdcf0c7f44134a998f1cfcd9b9af1df04ac90095b958fda27ef7e6638572b6df5478b99365d" ) );

        TEST_EQ( test<sha3_384>( 7, str1 ), digest_from_hex( "8a9b1a3f5e568ccd6c2a6a07ba3544104b647da53f0d4f1958d434c09772ce96aa1995fe08c76fc9d8a18339c805d1cc" ) );
        TEST_EQ( test<sha3_384>( 7, str2 ), digest_from_hex( "a2f38501b0cc49e244cbabfc100297ec5d0671937caa7ab891ac657eaf3f553e39bdb0f073fc8c35235556827029fbb3" ) );
        TEST_EQ( test<sha3_384>( 7, buf1 ), digest_from_hex( "6f5026363c4913655b545523833d4b2ca835e011225c241e7dfd409bf7f5bdaf676d2a133b2609fdf8ef988e71b2da45" ) );
        TEST_EQ( test<sha3_384>( 7, buf2 ), digest_from_hex( "6ff218f67b58d7ffb41bba299f61378ffdf09d190e4a10c99e2d77c308b9cc644383050591f577ae124f03b53da70366" ) );
        TEST_EQ( test<sha3_384>( 7, buf3 ), digest_from_hex( "b8caa3113e315b2a1fd69ffc162ef13b9eeaf8e3ea7e02e1c652bc250d0ece626b044901efc294f1cb182a1804a34c0e" ) );
        TEST_EQ( test<sha3_384>( 7, buf4 ), digest_from_hex( "bdbcdee009105091b6f755a16320dec2969a473f905e45b32ba6c83c6b2499c21c54bc9e75a331d10af71f0535296ef2" ) );
        TEST_EQ( test<sha3_384>( 7, buf5 ), digest_from_hex( "b62d536e16301d01fe5ab227bd3f27c78fef6e4bd8c1924a627e5244f0d4a0f6daaa1607525095736b050d1deec34f5a" ) );
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

        TEST_EQ( test<shake_128>( 0, str1 ), digest_from_hex( "5881092dd818bf5cf8a3ddb793fbcba74097d5c526a6d35f97b83351940f2cc844c50af32acd3f2cdd066568706f509bc1bdde58295dae3f891a9a0fca5783789a41f8611214ce612394df286a62d1a2252aa94db9c538956c717dc2bed4f232a0294c857c730aa16067ac1062f1201fb0d377cfb9cde4c63599b27f3462bba4a0ed296c801f9ff7f57302bb3076ee145f97a32ae68e76ab66c48d51675bd49acc29082f5647584e" ) );
        TEST_EQ( test<shake_128>( 0, str2 ), digest_from_hex( "7b6df6ff181173b6d7898d7ff63fb07b7c237daf471a5ae5602adbccef9ccf4b37e06b4a3543164ffbe0d0557c02f9b25ad434005526d88ca04a6094b93ee57a55d5ea66e744bd391f8f52baf4e031d9e60e5ca32a0ed162bb89fc908097984548796652952dd4737d2a234a401f4857f3d1866efa736fd6a8f7c0b5d02ab06e5f821b2cc8cb8b4606fb15b9527cce5c3ec02c65cd1cdb5c81bd67686ebdd3b5b3fcffb123ca8ca6" ) );

        TEST_EQ( test_hex<shake_128>( 0, buf1 ), digest_from_hex( "84792aaa71ccdac7b1258dca8e1e27bd1651259d360ffeec5058e9549c0a035ba4f67f51c9743da73196851f092f6c3887370878a52c11fdeac7925ff625123ed33ba8baa219ac5faca45991958424e83032e27c668fe724dec1377b84042c45cbf84986991d3cee09384e7018115069aae8aad7b167d9ee8373446d6471749f5696fb912ca025b8f01e1d4e53e153afc05ac79a6a0241f87692a048ceb6f35786d5253a61fd2043" ) );
        TEST_EQ( test_hex<shake_128>( 0, buf2 ), digest_from_hex( "66ea117321913ac62fe304667f1605a969530b176106e7de54e0f00c3cc298cff6e7814ade6eb3f4570420ab707f742e32b2c043256eb5353ac9826cfcac21ad6366ef723a212b35ecd7b60d53857a4fb95fecb2ba1dad4d441b8c7c08fe67d80cb859bb6eb717d4aaeeca58abb1eb946daa8bfee80a35211b01f72775c69e2303968384b28ba271d9c54a62042dc956be30b39a6e784d4c40ace69a4ac7502bd1b3d2fa2d2c0e85" ) );
        TEST_EQ( test_hex<shake_128>( 0, buf3 ), digest_from_hex( "12a19ff97cc82290a1b656411acee5366017ae885ce95949754673942f851e32d938fa443c04144b4a0b1d503a3cb54c59e67ce4c96f0c2a52ceaa689d6cbf13772f06f0c5fea12466cea9f9d464e248059c510ab310d70954c0711473d0cdee4fb75828ec4909920d98f78c7b28ff9dde77d288cb0d3d84cfca4f7d79b031f799b6ed0e6df1b07882764bdeb2d5986bcccdc1a4466155c610b0926343d7193e66384b9216438748" ) );
        TEST_EQ( test_hex<shake_128>( 0, buf4 ), digest_from_hex( "1b00bff40677f82d5337b921b0f1e6f8c974a1bb71bc30501c0fa8f5c7fb17458b91ec4140cde25b622088a40718cd01269d67084fdd4fa4c1e228c1ada0933c870de30e3a6498d7e3a77c08139d49a376d42b54b6c3eba9014bbbbeddbd851554921581b553cb40b5443664b3df435a05f007603f5941b2532cda65626891c40535718df2ad53b514888df05e6fd4e79e8b1523ff3068fd85d19ca58261ca885281136f7d52c78b" ) );
        TEST_EQ( test_hex<shake_128>( 0, buf5 ), digest_from_hex( "a0cd08ccce81ad3a6378bc520ef81b73562cbc65843800d7a145ab97bae6165470aaadb8a7cd38611eb00b98831855249a42fad0b2d9f32c872276bb3f60770ad6effa643acb052202dfa3b34bd230e24f5ad4d3e316d4674e69721c9754afafade6caa8e73eb69f8d8cfab15a12622b7ab4201dbb2790b701cb5b28f5f0c63739634f8919e4a3ab0897822f4e786abffa7ad7b31e42d0ae2587cd72b1a9b410b3bb1fd488678cbb" ) );
        TEST_EQ( test_hex<shake_128>( 0, buf6 ), digest_from_hex( "d7e7b905b3f6657b9a481fc04231aa4b2bcadb5c4dd217c85c3744389e1bfa7281df9c9efe67fd0f9f81946c44b7686207d9cfb920e1e4b062cb76d7cb40c812deef7b759e414342819a72c17e2aa130ad19288718003beaf6ce82c17199c0edc98854f6c1aeac1be4f880669f6960796c5832a521d8e3b80c39c782cf14bcd0031abde8604d3e060c8b5abbee3cb65737fa84b627710b51e0777de38371f48efd4da3d438036bde" ) );
        TEST_EQ( test_hex<shake_128>( 0, buf7 ), digest_from_hex( "cb843576fe43fd4d3f256d1c748e7fdc0abf361a6ee7ce089a830acde839153fc12fd4fef14294bd0510c895bf6a2a1b79f2532481ae210d86ec3620ba9e64b20c90a83ce7cc88174288a2d34a806b54edfd6cd7a2bd458ebeb0bc9522a731349d3f99f82ea92d5f4c82cb1b46c081e9f690b18b6e4533a684365a4da4876bd520b477dd8e382d3de20054fb5a0e959567bfb16c64a747053dae011927345478f3eb8719cd9d8e35" ) );
        TEST_EQ( test_hex<shake_128>( 0, buf8 ), digest_from_hex( "56d3af851684bffd23ca0ae8ce7497f70063846ced15a05d7081edd5f0de5ba7ba2252c0f3dc9efb9c2c055474794ce1795a33dd7a3e1169eb0387d7a793e105f5ff14eb9c62057893c68a95aa82c2154e6fa3e8f3f15a7e64e9c14af818c1c177b57f484f3ffd8fba4feea630039fd75c684074e8a8f1e693f51bd4f8c38de23b609499fae30973fbb96b2547cc09db12cb20f6a7e48cfd1e352e9fe61549f383d2f3ce50e72ca0" ) );

        TEST_EQ( test<shake_128>( 7, str1 ), digest_from_hex( "58c000f0e7f6775f731fbc3c5ac0758f24cf8b1164c2149fa61a0ff50e4cf2ac300fd4c889d3bee80a8d8e7f6e8c8e2e7c36f6e605fd60768006ebb08921d2ffa299f67cc4cecd9dbe0bebfab10c4dad2da06a83e93498e1ca2bc44e6a93c0f372d4fca4095a1c2de2b5a8bcbeeb341e225ff46806aa886ac85e37c05eace5426a86ef049f48c5beed7d710023e003007775a320b997ecb7f55abe2fffeac5d247da9a91ae3a17f9" ) );
        TEST_EQ( test<shake_128>( 7, str2 ), digest_from_hex( "8204f09577c5fce578e50f701febee10ae153b58cc56699a8cf24e5fa9f84609c251756303d5e51cb67963208e818ca776308365878fb67ed6b81c135884031f0a77e399d0ce2e41cc68e313521a68878a9200ce69caa25c608bd63a49c51c31e5a7398865a5f029d4f4c7b9b8f13d72ce882b85f241eaa535397ad30dd3483dd61c13465932844d440674bd51af7c3e364459c6e9d4ca9ac4430477f716f69eb0f9e6090a808f2f" ) );

        TEST_EQ( test_hex<shake_128>( 7, buf1 ), digest_from_hex( "2f8525e19c162bd525d8609647a4ea67f74c94bab3e862ce05f3250c8ec07da6c8490e18704ef6941f2821c7a943672bc31b292cb907a51a35396c9e03857464362b444be5a821acbf2fb25b461af23c2cc6eee3f8fa8fa441b8a193c2f56f62995e931eec824b2360702e1f46850ed3d967a4e0e1803045925d416596080faa196d373ced78748b6d6ea75286c83233a65ca2d7d20c2dbfee841a347c95670a04fb57137f09e306" ) );
        TEST_EQ( test_hex<shake_128>( 7, buf2 ), digest_from_hex( "3701be9e397268c48794e531110f2e40a7626f9e4d78a24ffd58f633959ad10601921d343451f6305dcfb41efeb087b49ebb6106869aab3f5cc682193a5d2f971940402a10c026990e987f5afaf26fdbcb6cd5ff4e178b8546d811d23aaf4f8948c163c79b1b88e6c24be61e4bc4da58e82898fe4f22ebc306b45216312147f23fc208eba64d6e0def9b779dfa6d092305a04fe9790303f0d4df0c10361b00abfdb59c74afe635c5" ) );
        TEST_EQ( test_hex<shake_128>( 7, buf3 ), digest_from_hex( "16fa23f243692ba51cce8caf3e8ec5f6dacd9089cc95adf565d03fde9fa1d8511d93bfc77b0ee4bca0c85cc6953ec248a7d1672bab984ae97e4efe9695a2093de30e293706474d2599d39e5cb4ff73ddf26773c11948702c3e5cddbb91c6cf1297993993c305f3379bf93d89d19dddcf0ecb9d3b7f4d810fdb4684cfa641e431906efa152a450c4670541b4c2fc8d34ce1344cd04b62023774c6e07dacd6ad67d616607e27f09319" ) );
        TEST_EQ( test_hex<shake_128>( 7, buf4 ), digest_from_hex( "1283c8df4bc360faa643887d82a8d6703804695e2b388329767bc8b24d8c90a40a56179342b76962780a9e73b32f6cbbe18659afbe8e81644fb3b36143b0f648f3810a9c0201dc9c69e67b46441b52255fe6a8da27a8fea64c195d1216a42987854b662e72aeeefb980297e645961a7aadc53d2d3c06d6fe13d0f186785f84ac8d2e11cafd65291def3444c8bfd4f48acd30e98033207555cb0fff57db778fba669d832a903fcf5a" ) );
        TEST_EQ( test_hex<shake_128>( 7, buf5 ), digest_from_hex( "66717f9537dad600929c5298511d68cafedd49eefe58eac300b124773c69f2b9493782d24fbd3c06528ce6ed92f325638cb46b687c6ce50fda3f83a12c90ca7cff8a342856e851462524c141ac6ca7c70472579e03d7928818b1a4348a33e9b650995e07ec6cdd8f1983e2654a9171aa9ec48a7727185c25a9dff6afbc9c26fe73fd835e44fca4fc8868d3a518a02e25f52d81c45b80c62fdf4d211b359a0b1d06805867e626f10e" ) );
        TEST_EQ( test_hex<shake_128>( 7, buf6 ), digest_from_hex( "365401899349deff745f63bdd3f5e893e33444064d9310b8401e6bc9840423b3e7f4c775c85ae33f395654f4a46035ca2c6a4c953a8830fee5a98bd8c59db490381a406514954c02c96d53b945074b9354cb926e7a1b260eda18907f441f62917b84809262d78b404999ceecdfbd558ed397999ea0f9e7b68e608f317dbc0682468d1520d1708cf8c347dc10ca8bf4129b5ec9c0c178d4d01d5526b0ce0dc5515226d1af47d4d729" ) );
        TEST_EQ( test_hex<shake_128>( 7, buf7 ), digest_from_hex( "401d71589b1c62bf0fd83be996682fef774a6e7a2fa37ed5fc2e70aaafb7983d0aee9fb33a1cfae7c11edf6dfe40ee49adf969c929e63130bc31be26237c99ed2cc482adb0f91a40e071934e3fbf44419a94fb0acae9f094b3724f7db212091d09165f01e7ce4c2f826d8a10e747f242b60b3a26e99d01dee3ec0ecca8d553c42f674b68bdeddae97d389cb2cff43058a1d4a2ae72b4373f2043d51ffc7daba0ecd8f46c4ef087ca" ) );
        TEST_EQ( test_hex<shake_128>( 7, buf8 ), digest_from_hex( "f7901899d8bd983cbefa25ee3064d533a2e91aec5f2e091df0dbcd33e67524030bf359ca2265f7b030155137fc07bc1820f142d7e26262508d326f53aec09b76b74bb4557d1a91e493523f259f0928ac0bd219363507f437fba054f86ececa8e0a39493f0da83123b32be8f744470e5b26f34a42ebd5f218a3bacbec92c697f3da02aad2f37941f5f9bfad45304e58f4b4e975c577c9f1a570b41745210318ee2da7895e450284a7" ) );
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
