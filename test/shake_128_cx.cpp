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

        TEST_EQ( test<shake_128>( 7, str1 ), digest_from_hex( "502717a8480a3cb9e2a151854f3cdce6cb301227a4e0e37e03ee760454ad20d9acae1fa8939ca67210b0f817483636a9f5d78173cf3c121d59e2f83f04a5fe91bac770a724e838c3fbac03e753cbf64d3455bed7b5e355aa082b8829da3d3b77d5ef004c0922067607530895703a17cd34b0fc7dc330ea132bddf9d8b9878cd9f9c0a1d86bfac05ab0cbc6d97ac4c299655b3ce0aa0f4a9ba18efa28cf526dc443a959c9b950df5f" ) );
        TEST_EQ( test<shake_128>( 7, str2 ), digest_from_hex( "4df5c9ffeeb6864010e611151b5d1b6a67804cde351ea270c0a33debf5d2d3c03d9e23b69e642b7890bf809d181b97c138b6f86ca597fd675f2f81fb452e44445c25bccb0cb115ea596925088dba2c4810ae69cdcf1d9886adf759d80498792bc4ac2c2dae5b6dcf992b634387092d027845389b8a1b3cdc76678c35da210b4d525f1f9fa7d321dc413da7c37b6fd98405517cc71d7d759bd5746619df2cdb429e0d9842d8603e8f" ) );

        TEST_EQ( test_hex<shake_128>( 7, buf1 ), digest_from_hex( "c9b36240e14c4afc18c9826338fc805e50f55b20602c7ab4f2f1be9504b033b5f74a4b157155fd5644e9a14135750fe73e76decb3e0c3028b19afaaa1f6de00d249f46a7b20d42c19f8c16adbab3b636b26961a1144de38a3f8fa7d13bdf0d54b4e87111d552b25889d6602c7eb6c70a2b8cc5d11ec90c7923c667e7f77002bf4343c7f71bef8957ecd1baaec58acb00144cb22ec96b334255a0f9f6426d2568dded9a2ba2b43e57" ) );
        TEST_EQ( test_hex<shake_128>( 7, buf2 ), digest_from_hex( "19b50182a2b5d10f87793a8b7aba1e58744a9f74a524001f28f62ddcbc6350b53bc8b978ef7ef628743c5621bb5842dbf52469c9687b597fcc0766eb4fc56f39b06a5d53d4a31e38e59c07d5198fb119236cda495887f79cdb53eac6826e9b23306daf3cb019884ff362d01b9f91317fd68b6388b0a92eeeb2217796b018d0e687e48f4517a963d2fff4b6c61b3eda9c7142db27fa7025d1b7a90cefc7ca5995a4fc7934073ef2cc" ) );
        TEST_EQ( test_hex<shake_128>( 7, buf3 ), digest_from_hex( "7d5e9c4729066b2ff430491fc730514fe9ad39694a69a037f00ae4f2dc0fc84926ce4c7d666617acf6d81ce2b443ccd9be0969f29c048478648c67ca23b576a2b35cf3dc6fb82769c224fa466c7a60b8bfb7a98c55f20bf9116ea1249f81d3df4999b64cb8ea637ca59c089a4bca733b0be27633361c26e6e2767cb438f92fbab53a7427014e349be5f17f88c5723ac6e81d3f097b58cfd8750d19ce69db33839426fb8b019eeeb4" ) );
        TEST_EQ( test_hex<shake_128>( 7, buf4 ), digest_from_hex( "414f37e898f5d6b7c2d7916bbccdb31e9d6dc5b8f47cdee0c99439a902a7126792a7124c990e40954d34e320caadb316efef8f13b9942aafdeba95f8a51c1e6d8c2d1178bdb4ffb2c5ceb77dc1a1acec07fc89581f9ee87dcbd0d883491a947394375131ead6bd2bcf04d8741d33c5a63cd6de5d3d4b163faf6953a0a8e41a4e96a17002737194bbabe75ee9d60e4df87e61e870a8d4bdfed4815543793db1359a756818ec6a2520" ) );
        TEST_EQ( test_hex<shake_128>( 7, buf5 ), digest_from_hex( "eb4b48121025619d198ae754276b2e018b87b5ecc4b6fc52d6aaffd716fbe9886b75439e4a5e12467c10fdcbfa0b3f99234c24d7a5e54187dc103dd4267fc5e7257234c1a17293cc85771d7d7ef7f12c30e1e94de760412bb8463db629ff6204d963bcd69ea5c4dacf808829c4100d56d1088b008784a78a29e7d5a3bfd1125ef29a035f22fd53ea63ea160ab9b1463afd333e583df0fd1e5713de2dc6633f49bc10e962878cbadf" ) );
        TEST_EQ( test_hex<shake_128>( 7, buf6 ), digest_from_hex( "895b45768ab41accfd8bc9fb30f27b5c7310732ca65b2ccbc9ec52f803df54f9bb3d79d859a08d6b465b3132dc611faa01707b5f2c096a73b05b13df5635b44e0f8f6a6df17319fea2cebbb17dac6d8221789abd1ebba505ad641e0ecd4d2ff19583a5aa4bc6771e8233ea17f592f21b05cf11083e46599d25165158dc1a92b9e79fc3fbb5095b5c3f473f97aa37fc5fc4ad66d1ca10c05869d90a9fa57adbda3687b7776e27b092" ) );
        TEST_EQ( test_hex<shake_128>( 7, buf7 ), digest_from_hex( "6111239f4885350828854a92d1a7595b639a814aa98127d6173aebb222c20081093d4f2f0c4b2ce36fea948bc505e6f83926c4d69e691688a5961c457b9bc6712a5ac6d85269c2bd73408abe3185d2cdd993e13f6581dfe5b08c5eca61187f9d7344013a8c39cd9c49e26e2941cf8d65c16c718bdfb471660a03f2c78a217bc165f4df9f9d17653008e6cd843de923c45a261432e556e8743d1987106bc0296992169323ee09701e" ) );
        TEST_EQ( test_hex<shake_128>( 7, buf8 ), digest_from_hex( "0167c8d82eefa3d2f37ddabceca09c90e0e47bfe6c3d223cd853596d183c6512b8817289310cf4597afed3eda90c8a50c7c4511bf183e754d89561be3c6e3fa89d61dbd72ef07ff2312239354798997ad39d7b38aac891ec0396ff478e49980569db1d41b223bdf6dc2c7ef633babae3649a3c854a9734ca8a026fca072a3ab585f76b45ff416287494d8396e5846957228c6e288b6ae4cf99ab49c4015529cd8cdc005d4efe5a3c" ) );
    }

    return boost::report_errors();
}
