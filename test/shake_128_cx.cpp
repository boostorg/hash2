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

    return boost::report_errors();
}
