// Copyright 2025 Christian Mazakas
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/hash2/sha3.hpp>
#include <boost/core/lightweight_test.hpp>
#include <string>

std::string from_hex( char const* str )
{
    auto f = []( char c ) { return ( c >= 'a' ? c - 'a' + 10 : c - '0' ); };

    std::string s;
    while( *str != '\0' )
    {
        s.push_back( static_cast<char>( ( f( str[ 0 ] ) << 4 ) + f( str[ 1 ] ) ) );
        str += 2;
    }
    return s;
}

template<class H> std::string digest( std::string const & s )
{
    H h;

    h.update( s.data(), s.size() );

    return to_string( h.result() );
}

static void sha3_256()
{
    // https://en.wikipedia.org/wiki/SHA-3#Examples_of_SHA-3_variants
    using boost::hash2::sha3_256;
    BOOST_TEST_EQ( digest<sha3_256>( "" ), std::string( "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a" ) );

    // The block size of SHA3-256 is 1088 bits / 136 bytes
    BOOST_TEST_EQ( sha3_256::block_size, 136 );

    // selected samples from the download available here:
    // https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Secure-Hashing
    std::pair<char const*, char const*> inputs[] =
    {
          /* expected hash */                                                 /* input message */

        // 8 bits / 1 byte
        { "f0d04dd1e6cfc29a4460d521796852f25d9ef8d28b44ee91ff5b759d72c1e6d6", "e9" },

        // 16 bits / 2 bytes
        { "94279e8f5ccdf6e17f292b59698ab4e614dfe696a46c46da78305fc6a3146ab7", "d477" },

        // 72 bits / 9 bytes
        { "fd09b3501888445ffc8c3bb95d106440ceee469415fce1474743273094306e2e", "fb8dfa3a132f9813ac" },

        // 104 bits / 13 bytes
        { "69dfc3a25865f3535f18b4a7bd9c0c69d78455f1fc1f4bf4e29fc82bf32818ec", "9c76ca5b6f8d1212d8e6896ad8" },

        // 136 bits / 17 bytes
        { "6c2de3c95900a1bcec6bd4ca780056af4acf3aa36ee640474b6e870187f59361", "266e8cbd3e73d80df2a49cfdaf0dc39cd1" },

        // 192 bits / 24 bytes
        { "cdfd1afa793e48fd0ee5b34dfc53fbcee43e9d2ac21515e4746475453ab3831f", "cf549a383c0ac31eae870c40867eeb94fa1b6f3cac4473f2" },

        // 328 bits / 41 bytes
        { "e02c1b197979c44a5a50d05ea4882c16d8205c2e3344265f8fe0e80aed06c065", "bdd0252dec5b798ef20e51791a18e8ca234d9bfde632a9e5395337a112dd97cdf068c9f57615424f59" },

        // 600 bits / 75 bytes
        { "60d80f1c703dad5da93db222fb45fb7fa768c8aa2787f4b81f1e00365b8f49e2", "a0dfaecd3e307c5ddf9a93603f7e19725a779218734904525b14586ff0ce0425e4efe7e1c06e745c28ed136f6031c4280fd4061d433ef700b6d1bc745064231fecf387015f94f504b6ad8c" },

        // 616 bits / 77 bytes
        { "51cc71b6934afcf28fa49942b76323f36cd6a0aecc5a0e49c10994ddcabdbb80", "d65b9f881d1fc7f17d6dd429faca8404e6ce60fba7d89b7fba003c8ef84d8083182979327611fc341291ba80dc70ad3b2f28b6d29b988445e7fdb7c6561f45822ac81dbf677a0b27d961dc6358" },

        // 856 bits / 107 bytes
        { "8cc4d39b2f5ba0bc9d2ee2a8777cf08533e60cc69b65a7b31c5c2121193aa31e", "6db2a43a229b10c3629249fc5136468b4d84df7b89ec90ebf7aa7a036c53aa2dffae9e81b2c60580543dc706a5e3457abc87e248a60ec29150c2d221a6ec08a1fda4ec0daee8576904ec7ab059b1230e7bd93c4e55ba9496cbb1e352e5b8086e303b94c861288ce53c466b" },

        // 1048 bits / 131 bytes
        { "c163cd43de224ac5c262ae39db746cfcad66074ebaec4a6da23d86b310520f21", "da11c39c77250f6264dda4b096341ff9c4cc2c900633b20ea1664bf32193f790a923112488f882450cf334819bbaca46ffb88eff0265aa803bc79ca42739e4347c6bff0bb9aa99780261ffe42be0d3b5135d03723338fb2776841a0b4bc26360f9ef769b34c2bec5ed2feb216e2fa30fa5c37430c0360ecbfba3af6fb6b8dedacbb95c" },

        // 1080 bits / 135 bytes
        { "f82d9602b231d332d902cb6436b15aef89acc591cb8626233ced20c0a6e80d7a", "b1f6076509938432145bb15dbe1a7b2e007934be5f753908b50fd24333455970a7429f2ffbd28bd6fe1804c4688311f318fe3fcd9f6744410243e115bcb00d7e039a4fee4c326c2d119c42abd2e8f4155a44472643704cc0bc72403b8a8ab0fd4d68e04a059d6e5ed45033b906326abb4eb4147052779bad6a03b55ca5bd8b140e131bed2dfada" },

        // 1088 bits / 136 bytes
        { "4beae3515ba35ec8cbd1d94567e22b0d7809c466abfbafe9610349597ba15b45", "56ea14d7fcb0db748ff649aaa5d0afdc2357528a9aad6076d73b2805b53d89e73681abfad26bee6c0f3d20215295f354f538ae80990d2281be6de0f6919aa9eb048c26b524f4d91ca87b54c0c54aa9b54ad02171e8bf31e8d158a9f586e92ffce994ecce9a5185cc80364d50a6f7b94849a914242fcb73f33a86ecc83c3403630d20650ddb8cd9c4" },

        // 2184 bits / 273 bytes
        { "cb5648a1d61c6c5bdacd96f81c9591debc3950dcf658145b8d996570ba881a05", "b1caa396771a09a1db9bc20543e988e359d47c2a616417bbca1b62cb02796a888fc6eeff5c0b5c3d5062fcb4256f6ae1782f492c1cf03610b4a1fb7b814c057878e1190b9835425c7a4a0e182ad1f91535ed2a35033a5d8c670e21c575ff43c194a58a82d4a1a44881dd61f9f8161fc6b998860cbe4975780be93b6f87980bad0a99aa2cb7556b478ca35d1f3746c33e2bb7c47af426641cc7bbb3425e2144820345e1d0ea5b7da2c3236a52906acdc3b4d34e474dd714c0c40bf006a3a1d889a632983814bbc4a14fe5f159aa89249e7c738b3b73666bac2a615a83fd21ae0a1ce7352ade7b278b587158fd2fabb217aa1fe31d0bda53272045598015a8ae4d8cec226fefa58daa05500906c4d85e7567" },

        // 3280 bits / 410 bytes
        { "095dcd0bc55206d2e1e715fb7173fc16a81979f278495dfc69a6d8f3174eba5a", "712b03d9ebe78d3a032a612939c518a6166ca9a161183a7596aa35b294d19d1f962da3ff64b57494cb5656e24adcf3b50e16f4e52135d2d9de76e94aa801cf49db10e384035329c54c9455bb3a9725fd9a44f44cb9078d18d3783d46ce372c31281aecef2f8b53d5702b863d71bc5786a33dd15d9256103b5ff7572f703d5cde6695e6c84f239acd1d6512ef581330590f4ab2a114ea064a693d5f8df5d908587bc7f998cde4a8b43d8821595566597dc8b3bf9ea78b154bd8907ee6c5d4d8a851f94be510962292b7ddda04d17b79fab4c022deb400e5489639dbc448f573d5cf72073a8001b36f73ac6677351b39d9bdb900e9a1121f488a7fa0aee60682e7dc7c531c85ec0154593ded3ae70e4121cae58445d8896b549cacf22d07cdace7625d57158721b44851d796d6511c38dac28dd37cbf2d7073b407fbc813149adc485e3dacee66755443c389d2d90dc70d8ff91816c0c5d7adbad7e30772a1f3ce76c72a6a2284ec7f174aefb6e9a895c118717999421b470a9665d2728c3c60c6d3e048d58b43c0d1b5b2f00be8b64bfe453d1e8fadf5699331f9" },

        // 4376 bits / 547 bytes
        { "cb1c691c87244c0caf733aacd427f83412cd48820b358c1b15dd9fadee54e5af", "2a459282195123ebc6cf5782ab611a11b9487706f7795e236df3a476404f4b8c1e9904e2dc5ef29c5e06b179b8649707928c3913d1e53164747f1fa9bba6eeaf8fb759d71e32adc8c611d061345882f1cdeee3ab4cab3554adb2e43f4b01c37b4546994b25f4dcd6c497bc206865643930157cb5b2f4f25be235fa223688535907efcc253bcd083021407ea09cb1c34684aa0c1849e7efe2d9af6938c46525af9e5afb4da6e5b83da4b61dc718672a8090549cbe5aadb44f5bc93a6b3fbdc2e6d32e2eaaae637465179ea17f23ad1e4f1ebc328e2c6dc90c302b74a1edbbb0676c136b269d70c41040a313af06ab291bf489d9700950b77f207c1fc41884799931b3bca8b93331a6e96b7a3f0a8bd24cdb64964c377e0512f36444bb0643a4e3ecb328194cd5428fd89ede167472a14a9bf5730aff1e3b2c708de96eff1ebaaf63beb75f9c7d8034d6e5471e8f8a1f7efce37793a958e134619c19c54d3d42645f7a7263f25471fbaae8be3ea2fbd34ec6d7aacd7d5680948c3cd9a837c9c469a88f600d95829f4d1e4e4a5ef4ed4623c07815a1c33d9fb3b91333ff04eac92806a68a46cf2e9293f8bff466ce87fe66b46fbff7c238c7f9b2c92eb2fdc7d8084167f6f4e680d03301e5c33f78f1857d6863b1b8c36c7fce3e07d2a96a8979712079ae0023a1e3970165bfcf3a5463d2a4fdf1ca0e044f9a247528cd935734cb6d85ba53ceb95325c0eaf0ff5cd81ecb32e58917eb26bfc52dba3704bf5a927fee3220" },
    };

    for( auto const& input : inputs )
    {
        BOOST_TEST_EQ( digest<sha3_256>( from_hex( input.second ) ), std::string( input.first ));
    }
}

static void sha3_224()
{
    // https://en.wikipedia.org/wiki/SHA-3#Examples_of_SHA-3_variants
    using boost::hash2::sha3_224;
    BOOST_TEST_EQ( digest<sha3_224>( "" ), std::string( "6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7" ) );

    // The block size of SHA3-224 is 1152 bits / 144 bytes
    BOOST_TEST_EQ( sha3_224::block_size, 144 );

    // selected samples from the download available here:
    // https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Secure-Hashing
    std::pair<char const*, char const*> inputs[] =
    {
          /* expected hash */                                         /* input message */

        // 8 bits / 1 byte
        { "488286d9d32716e5881ea1ee51f36d3660d70f0db03b3f612ce9eda4", "01" },

        // 16 bits / 2 bytes
        { "94bd25c4cf6ca889126df37ddd9c36e6a9b28a4fe15cc3da6debcdd7", "69cb" },

        // 32 bits / 4 bytes
        { "0b521dac1efe292e20dfb585c8bff481899df72d59983315958391ba", "d148ce6d" },

        // 72 bits / 9 bytes
        { "e02a13fa4770f824bcd69799284878f19bfdc833ac6d865f28b757d0", "b29373f6f8839bd498" },

        // 152 bits / 19 bytes
        { "765f050c95ae3347cf3f4f5032b428faeab13694e8c7798eafb82475", "f4e68964f784fe5c4d0e00bb4622042fa7048e" },

        // 368 bits / 46 bytes
        { "23929753ad07e8476e7bdac8a0ca39e9aac158132653be10ebeeb50c", "b3008f6f567d1eed9ab5b3bbce824d290e66f66bcfcff7f9b8994835b4d54a4e45c9b8651b37dbefe5e3fe5b674f" },

        // 432 bits / 54 bytes
        { "5963b41b13925a90c9e8fbcded9a82ade8aae36dee920199f6d6ac7f", "c9aa3d0f6d878db11235e7b028f8d67e2ce26eee718f308e21132e377e3170e26ece95bd37a4bd7f873ba7f8b71517ec50297b21cf94" },

        // 488 bits / 61 bytes
        { "51d0cd33fd6579b05c366c6fcc653638b7b13b62798b99b36792cdc4", "18419a8498d4e9bfaa911748186c5753d5da5aa033371ffc56650d0ae9b73f430f0d1f3c9d40362786c0429d977b899b64016eca82e64203f6685c12ee" },

        // 704 bits / 88 bytes
        { "9e614fc139645e158cd1b216e2623e586242af64f8483e6fca20ed4b", "31857bb4e82497b526e426de6920a6063d02264d5249feffd14abdbbf03563d4c59ad1f7572c7d0efbc46a65dea9580bde0e387c9edce27cd9b20a46f62a70e6dd5f58e40aac3a22dfb6ba073facdadd58cd6f78c02bd219" },

        // 992 bits / 124 bytes
        { "6b40e5c86db3d9c384c22a46cbef5f8e8c427bb6bf43268edd918aeb", "6f057f91480fecee8a7e3879dbf8c52040f96f5929c6b8b6aea223b91843ddeba387a2288264df3d241d14b5b6bc7defe9bcf174f5060a88de1f86fff59fed52a3e574f2620922dc0c12316e5869b779a18e8697ea0a50bf20a50f169ed8a308f785bd98efe6fdf4cac4574dcae9bbe5f3d7f56a11bad282fc9c84a7" },

        // 1144 bits / 143 bytes
        { "98ec52c21cb988b1434b1653dd4ac806d118de6af1bb471c16577c34", "0eef947f1e4f01cdb5481ca6eaa25f2caca4c401612888fecef52e283748c8dfc7b47259322c1f4f985f98f6ad44c13117f51e0517c0974d6c7b78af7419bcce957b8bc1db8801c5e280312ef78d6aa47a9cb98b866aaec3d5e26392dda6bbde3fece8a0628b30955b55f03711a8e1eb9e409a7cf84f56c8d0d0f8b9ba184c778fae90dc0f5c3329cb86dcf743bbae" },

        // 1152 bits / 144 bytes
        { "26ec9df54d9afe11710772bfbeccc83d9d0439d3530777c81b8ae6a3", "e65de91fdcb7606f14dbcfc94c9c94a57240a6b2c31ed410346c4dc011526559e44296fc988cc589de2dc713d0e82492d4991bd8c4c5e6c74c753fc09345225e1db8d565f0ce26f5f5d9f404a28cf00bd655a5fe04edb682942d675b86235f235965ad422ba5081a21865b8209ae81763e1c4c0cccbccdaad539cf773413a50f5ff1267b9238f5602adc06764f775d3c" },

        // 2312 bits / 289 bytes
        { "aab23c9e7fb9d7dacefdfd0b1ae85ab1374abff7c4e3f7556ecae412", "31c82d71785b7ca6b651cb6c8c9ad5e2aceb0b0633c088d33aa247ada7a594ff4936c023251319820a9b19fc6c48de8a6f7ada214176ccdaadaeef51ed43714ac0c8269bbd497e46e78bb5e58196494b2471b1680e2d4c6dbd249831bd83a4d3be06c8a2e903933974aa05ee748bfe6ef359f7a143edf0d4918da916bd6f15e26a790cff514b40a5da7f72e1ed2fe63a05b8149587bea05653718cc8980eadbfeca85b7c9c286dd040936585938be7f98219700c83a9443c2856a80ff46852b26d1b1edf72a30203cf6c44a10fa6eaf1920173cedfb5c4cf3ac665b37a86ed02155bbbf17dc2e786af9478fe0889d86c5bfa85a242eb0854b1482b7bd16f67f80bef9c7a628f05a107936a64273a97b0088b0e515451f916b5656230a12ba6dc78" },

        // 3472 bits / 434 bytes
        { "d61f04985026eee29d0f9700f8c5aea32ec2c23b1a9357edeb2be20c", "ab4f9d765085ccb474be6e2369568292532f6fa4dd9c50d02a7d8fab0fabb56a7f9680a2462c3753fafd3a252f9dddf1eb4a76835acfb59fc2a83441b8674f2995573697245e40549d2883f1d781a153b903e470f2f28e53e9646a66f7a5a7f0d5d9e6dd50e392be44867010c7ca77c1a5a2e1f00dcb82f589f759a1332b65c62766b9fa3483d399d7602a0969400642976e948d13243a8b89aa287ad5c230b47344d7783606aced3dfed86424abf7de77b026ce6cc35d20d1c500794332b0c1a1bc67dfc033c4c360a8a3aa5fd2f19d2db1bf3b807094b949900827e6438ef5991692b539d3c42227a6b362847e9d88a1b6855db7f58760d953690b26bd7258439a7f8409ae53137a3f2f14fa77a2a6bc0aa3bb7a19dd1c69554aae6c6703f3879057d3978c1a9d41bd3f492985aa0064f43fde2fa33ff6e1dfd4961e0aeacd4e3f412b4d35c0c864660d8779705a9c82bb824c405c54f429392e4da66ecfee7ef066139270ee9ccc83be5952ff5c84ffa8938f130cc52129ab825b6a5b585f01ebed13ce074c225f5b7d441cfc58c0c1039a2f127b3982ca7df546d4993027bd78ffb36ac08161063870d23f2df556b214" },

        // 4632 bits / 579 bytes
        { "5c029633dfd4cd9b6ce97e1d20783a41cd2235b03c38832b90f759ee", "1050e9023d6cef2477171a5af4875e12763dc68568ce1d9629e9eefda896b0a431bc3a5186e67d86324671a61afb5058558f03303dc4d98dc6907fbd0f662d6837b71d2b72b941835c6735ed8dc9734b6d75c68a49560182a7acf01dd7548fbccb4a7ad6296ca5e29d804d7874792eb367f6f7d9b40cfb3e7445296528e83e69957b399bd2870f6d9f9ba18ff893eb57ea1a7e66a66e3089d46412dd29aba2373c5442c29592e9ea07bf197941a011009fea43794605082a6f0f93cd032423ed69dd7d939b169a2280f59d80762808762d6d2e5abc58ecebee51947cdddc2a55c31dd3560c9b8addb04415eabfb137813eac285031ef4292f92ffd33e0591d35f31c2a5210a721e33fb4690045c508e977ad17a4cd5373e837b3b63a34256656a0e26cd0cdc4c7fbd64d810a30ebe275827ae59fb70be8b551e4a7d900c86b8073787f186e79e4ef1d184550a8ac2def071b7886189f08c1eecab9586cc8967a13e54d72194ee085bb1c9e19d48e521dad96baa417983ad3b76ed2fea2af4e0e2a31bc26f7cc2386596172afdc1bb7c71df59140dd7de8b554d7db14c704fa025d65b67b2f7a15b99ef0ab58e9dbc5b7ff40e7a191d6792d6c0dbde20650041f568722bae9e11fe833d02f5d2355e4b4fa7da3105c2c5504a7195eb0851ad32dbda2e72c4f87a7d9bf09e806b1117d85b6b6add56605e402af02a8c66ceacff439bbef1686f61755eba4bc9abb97f6f3dcf2ed38d6ec8dee29d0826be448603b73dc21c3b9b6d5245fa895636b70b9c6143a4b81d466bb91c08cd3e915bdeecc130c65" },
    };

    for( auto const& input : inputs )
    {
        BOOST_TEST_EQ( digest<sha3_224>( from_hex( input.second ) ), std::string( input.first ));
    }
}

static void sha3_512()
{
    // https://en.wikipedia.org/wiki/SHA-3#Examples_of_SHA-3_variants
    using boost::hash2::sha3_512;
    BOOST_TEST_EQ( digest<sha3_512>( "" ), std::string( "a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26" ) );

    // The block size of SHA3-512 is 576 bits / 72 bytes
    BOOST_TEST_EQ( sha3_512::block_size, 72 );

    // selected samples from the download available here:
    // https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Secure-Hashing
    std::pair<char const*, char const*> inputs[] =
    {
          /* expected hash */                                                                                                                 /* input message */

        // 8 bits / 1 byte
        { "150240baf95fb36f8ccb87a19a41767e7aed95125075a2b2dbba6e565e1ce8575f2b042b62e29a04e9440314a821c6224182964d8b557b16a492b3806f4c39c1", "e5" },

        // 16 bits / 2 bytes
        { "809b4124d2b174731db14585c253194c8619a68294c8c48947879316fef249b1575da81ab72aad8fae08d24ece75ca1be46d0634143705d79d2f5177856a0437", "ef26" },

        // 40 bits / 5 bytes
        { "ce96da8bcd6bc9d81419f0dd3308e3ef541bc7b030eee1339cf8b3c4e8420cd303180f8da77037c8c1ae375cab81ee475710923b9519adbddedb36db0c199f70", "4775c86b1c" },

        // 128 bits / 16 bytes
        { "f3adf5ccf2830cd621958021ef998252f2b6bc4c135096839586d5064a2978154ea076c600a97364bce0e9aab43b7f1f2da93537089de950557674ae6251ca4d", "054095ba531eec22113cc345e83795c7" },

        // 432 bits / 54 bytes
        { "947bc873dc41df195f8045deb6ea1b840f633917e79c70a88d38b8862197dc2ab0cc6314e974fb5ba7e1703b22b1309e37bd430879056bdc166573075a9c5e04", "dfc3fa61f7fffc7c88ed90e51dfc39a4f288b50d58ac83385b58a3b2a3a39d729862c40fcaf9bc308f713a43eecb0b72bb9458d204ba" },

        // 552 bits / 69 bytes
        { "2c74f846ecc722ea4a1eb1162e231b6903291fffa95dd5e1d17dbc2c2be7dfe549a80dd34487d714130ddc9924aed904ad55f49c91c80ceb05c0c034dae0a0a4", "fca5f68fd2d3a52187b349a8d2726b608fccea7db42e906b8718e85a0ec654fac70f5a839a8d3ff90cfed7aeb5ea9b08f487fc84e1d9f7fb831dea254468a65ba18cc5a126" },

        // 568 bits / 71 bytes
        { "9e9e469ca9226cd012f5c9cc39c96adc22f420030fcee305a0ed27974e3c802701603dac873ae4476e9c3d57e55524483fc01adaef87daa9e304078c59802757", "b0de0430c200d74bf41ea0c92f8f28e11b68006a884e0d4b0d884533ee58b38a438cc1a75750b6434f467e2d0cd9aa4052ceb793291b93ef83fd5d8620456ce1aff2941b3605a4" },

        // 576 bits / 72 bytes
        { "b018a20fcf831dde290e4fb18c56342efe138472cbe142da6b77eea4fce52588c04c808eb32912faa345245a850346faec46c3a16d39bd2e1ddb1816bc57d2da", "0ce9f8c3a990c268f34efd9befdb0f7c4ef8466cfdb01171f8de70dc5fefa92acbe93d29e2ac1a5c2979129f1ab08c0e77de7924ddf68a209cdfa0adc62f85c18637d9c6b33f4ff8" },

        // 1160 bits / 145 bytes
        { "e5825ff1a3c070d5a52fbbe711854a440554295ffb7a7969a17908d10163bfbe8f1d52a676e8a0137b56a11cdf0ffbb456bc899fc727d14bd8882232549d914e", "664ef2e3a7059daf1c58caf52008c5227e85cdcb83b4c59457f02c508d4f4f69f826bd82c0cffc5cb6a97af6e561c6f96970005285e58f21ef6511d26e709889a7e513c434c90a3cf7448f0caeec7114c747b2a0758a3b4503a7cf0c69873ed31d94dbef2b7b2f168830ef7da3322c3d3e10cafb7c2c33c83bbf4c46a31da90cff3bfd4ccc6ed4b310758491eeba603a76" },

        // 1744 bits / 218 bytes
        { "cd0f2a48e9aa8cc700d3f64efb013f3600ebdbb524930c682d21025eab990eb6d7c52e611f884031fafd9360e5225ab7e4ec24cbe97f3af6dbe4a86a4f068ba7", "991c4e7402c7da689dd5525af76fcc58fe9cc1451308c0c4600363586ccc83c9ec10a8c9ddaec3d7cfbd206484d09634b9780108440bf27a5fa4a428446b3214fa17084b6eb197c5c59a4e8df1cfc521826c3b1cbf6f4212f6bfb9bc106dfb5568395643de58bffa2774c31e67f5c1e7017f57caadbb1a56cc5b8a5cf9584552e17e7af9542ba13e9c54695e0dc8f24eddb93d5a3678e10c8a80ff4f27b677d40bef5cb5f9b3a659cc4127970cd2c11ebf22d514812dfefdd73600dfc10efba38e93e5bff47736126043e50f8b9b941e4ec3083fb762dbf15c86" },

        // 2328 bits / 291 bytes
        { "a6375ff04af0a18fb4c8175f671181b4cf79653a3d70847c6d99694b3f5d41601f1dbef809675c63cac4ec83153b1c78131a7b61024ce36244f320ab8740cb7e", "22e1df25c30d6e7806cae35cd4317e5f94db028741a76838bfb7d5576fbccab001749a95897122c8d51bb49cfef854563e2b27d9013b28833f161d520856ca4b61c2641c4e184800300aede3518617c7be3a4e6655588f181e9641f8df7a6a42ead423003a8c4ae6be9d767af5623078bb116074638505c10540299219b0155f45b1c18a74548e4328de37a911140531deb6434c534af2449c1abe67e18030681a61240225f87ede15d519b7ce2500bccf33e1364e2fbe6a8a2fe6c15d73242610ed36b0740080812e8902ee531c88e0359020797cbdd1fb78848ae6b5105961d05cdddb8af5fef21b02db94c9810464b8d3ea5f047b94bf0d23931f12df37e102b603cd8e5f5ffa83488df257ddde110106262e0ef16d7ef213e7b49c69276d4d048f" },

        // 2912 bits / 364 bytes
        { "f08819ec3a9a9806a1f55be4f0e56bce084e66fa271784974bf80e1bed7b2be559ebf5b6396ce52f7db7ef45543965f83064095a70328489178718b491a4100d", "8237ce9396ccde3a616754414cdf7b5a958c1eb7f25a48c2781b4e0dba220f8c350d7b02ece252b94f5e2e766189c4ac1a8e67f00acacead402316196a9b0a673e24a33f18b7cb6be4a066d33e1c93abd8252feb1c8d9cff134ac0c0861150a463264e316172d0b8e7d6043f2bbf71bf97fa7f9070ca3a21b93853ec55ab67a96db884c2113bea0822a70ea46f9ae5501eb55ec74eaa3179fa96d7842092d9e023844ed96f3c9fc35bbc8ee953d677c636fdd578fd5507719e0c55702fed2eaf4f32b35ec29a7a515bbc8bf61f9baf89a77aeb8bc6f247706c41d398cae5ec80b76abc3a5380001aea500eb31b10160139d5a8e8f1a976dd2dde5ce439a29dba24d370536a14bb87cf201e088e5e3397b3b61477c6a41e22a98af53cc34bc8c55f15d7924e7e32fed4d3c3ddc2ac8eb1dfc438218c08c6a6a8eea888b208f6092dd9f9df49e7ede8bf11051afd23b0b983a81bcc8d00f7d1f2b27cb04c03aeee59c7df23a17775ae5984eda7" },

        // 3496 bits / 437 bytes
        { "3a4c2c9284c90515cb34a0895d0374e87467ffbbc7c1dda3239893a12aeae3b9951169fe85605ef7aa2c483662f3a65c72ff12becde50c23ec6a2bc8864c27c1", "cfa6c0413dfc1a619417ac3f80fd38247b56941da8c2adf3ff70cc5dabed1875b0395d69d1200b73b1c7820b38868c5b38f52bf3514a96be12e27e34601d95d21c6f51c700b4edf1cac4b2079d487418a4cc5f34f815f469c4b44ef1a7dbaaa9597026c59260c9c22736c49d76ecf7430500b74866cbcfdb5e0fc4fa46cf5ee2b06363ca4ecba6d0104440348d191ec4a4bcbc9763152ffe271a69b805a0b9656970913dfd9e8c02cd16af33a878f083c926f48ab79b1db969fec493aef6c31accc1378867808440a5d5990490b07568bc66e9872904a0f46ae25ef4077b85ea217bdd12541a9472e2a9840e0d6ab55cc4a523f782f8c19774efbd41dad506bbafc90c438c14c780cab9fab9e74eb9452a0b29438a21878bcd4c6be4edac4e77bfd14a83d6152253a62e826de503880d37bf82d10924fab6bd23f04308a9660499bb223afcc5afd1bd2fa592d0322a9a30eab90bc7ac22018e99d2c8f573554c85b019d0c4cd75e359e5e9907082a8d660b353588b5f085486d89bd97bb32335cbd8b9adf7d57c72c078d9d08d9c09a70e43da1f1fe5b398ef08d2e06111d9a9b25a893a5d84cd643b0ffab8ef2755f781c1d6ca49" },

        // 4080 bits / 510 bytes
        { "e7ba73407aa456aece211077d92087d5cd283e3868d284e07ed124b27cbc664a6a475a8d7b4cf6a8a4927ee059a2626a4f983923360145b265ebfd4f5b3c44fd", "43025615521d66fe8ec3a3f8ccc5abfab870a462c6b3d1396b8462b98c7f910c37d0ea579154eaf70ffbcc0be971a032ccfd9d96d0a9b829a9a3762e21e3fefcc60e72fedf9a7fffa53433a4b05e0f3ab05d5eb25d52c5eab1a71a2f54ac79ff5882951326394d9db83580ce09d6219bca588ec157f71d06e957f8c20d242c9f55f5fc9d4d777b59b0c75a8edc1ffedc84b5d5c8a5e0eb05bb7db8f234913d6325304fa43c9d32bbf6b269ee1182cd85453eddd12f55556d8edf02c4b13cd4d330f83531dbf2994cf0be56f59147b71f74b94be3dd9e83c8c9477c426c6d1a78de18564a12c0d99307b2c9ab42b6e3317befca0797029e9dd67bd1734e6c36d998565bfac94d1918a35869190d177943c1a8004445cace751c43a75f3d80517fc47cec46e8e382642d76df46dab1a3ddaeab95a2cf3f3ad70369a70f22f293f0cc50b03857c83cfe0bd5d23b92cd8788aac232291da60b4bf3b3788ae60a23b6169b50d7fe446e6ea73debfe1bb34dcb1db37fe2174a685954ebc2d86f102a590c24732bc5a1403d6876d2995fab1e2f6f4723d4a6727a8a8ed72f02a74ccf5f14b5c23d9525dbf2b5472e1345fd223b0846c707b06569650940650f75063b529814e514541a6715f879a875b4f08077517812841e6c5c732eed0c07c08595b9ff0a83b8ecc60b2f98d4e7c696cd616bb0a5ad52d9cf7b3a63a8cdf37212" },

        // 4664 bits / 583 bytes
        { "a1416054e488c1e013762d642b2c63361b33e4fc528149845606de20998bf2afec05da53067477a3c27ebb3c0d24ad3dd6ed390335977f129f1b6b1526c0e0c8", "e34acd510cb32ca5f97298a3829244bb23322229fd7a07821dd40a8d01582d5558873f7c0a3d00d278e1872605dfe15dd558fbc1d518c19bfbc88803cf64a9f72af06fab3d673420d6f5c6f8df65108927ddf63066c980e77b153b1af79fdcb7dfec2785ae1a0fb69a151fbf180e1867a229dc1eb8768a912523eb7b83f00dbe01e22db2643cdabd4cab5824b9c14320cda47435d40829bf815a5fce7a3e8333183c4adb67b6de5c751e3acec966d7dc31b7881ac165a29a182361bae573873faa6146a8c07160bc9cd68d6650e41dc254c8de788777404971e4b7e7cb76610a41b9e9c07654ab04493b199357255dfbc04140f52f244af414062afe342f59bb64acfbcc9146065d04b5a5fee410dfaefd887439bf5607c58af282a72986b77b9ca243731a31b8ef56acfb4e028dea04910742ed42f4c0e25a0f8789b063c2716f038a5e18fc0ae1c688e12cb684c725062474b9bde6be730cc4014dd4aec3c667379834938f445cddb120400addbf38e449d0443a1446a1297bb79a9a4f02a10ca6359e94d2ae87218f803105801866b1dd2037c066a393389b72190c2ec72be5b9294421ad8f8b1c8ac8a8af561ad6f7482a3958c41b73c18cfa7231345a8b7ad63bcd4508318f560bc24c11450fb13df1b7f30916f8664cb5174c114702ea536735e205cbdabc567834c632363d1e0c428e0ddb4480966280914fb5500970f9d2dd2a6bded33aa43be7bd1b12ed46eba2792ed636fc2b8a542f242438b544f381fc4e7e296430c8baa3dad2bf685062793efe03d4b34d4d99ba7366e54f8fc9da59f54694d4" },
    };

    for( auto const& input : inputs )
    {
        BOOST_TEST_EQ( digest<sha3_512>( from_hex( input.second ) ), std::string( input.first ));
    }
}

static void sha3_384()
{
    // https://en.wikipedia.org/wiki/SHA-3#Examples_of_SHA-3_variants
    using boost::hash2::sha3_384;
    BOOST_TEST_EQ( digest<sha3_384>( "" ), std::string( "0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac3713831264adb47fb6bd1e058d5f004" ) );

    // The block size of SHA3-384 is 832 bits / 104 bytes
    BOOST_TEST_EQ( sha3_384::block_size, 104 );

    // selected samples from the download available here:
    // https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Secure-Hashing
    std::pair<char const*, char const*> inputs[] =
    {
          /* expected hash */                                                                                 /* input message */

        // 8 bits / 1 byte
        { "7541384852e10ff10d5fb6a7213a4a6c15ccc86d8bc1068ac04f69277142944f4ee50d91fdc56553db06b2f5039c8ab7", "80" },

        // 16 bits / 2 bytes
        { "d73a9d0e7f1802352ea54f3e062d3910577bf87edda48101de92a3de957e698b836085f5f10cab1de19fd0c906e48385", "fb52" },

        // 48 bits / 6 bytes
        { "21b1f3f63b907f968821185a7fe30b16d47e1d6ee5b9c80be68947854de7a8ef4a03a6b2e4ec96abdd4fa29ab9796f28", "5a6659e9f0e7" },

        // 112 bits / 14 bytes
        { "10f287f256643ad0dfb5955dd34587882e445cd5ae8da337e7c170fc0c1e48a03fb7a54ec71335113dbdccccc944da41", "cc4764d3e295097298f2af8882f6" },

        // 232 bits / 29 bytes
        { "6c0b3395e4c86518ab0a06267320ee9ec95e50385b7a2527ddaa1bd0ead262c56122d4f4eb08b0ae22b3ee7e6f44dd18", "7dd2d76fa054cf461e132e9ef914acdc53080a508cdc5368ab8c6224ff" },

        // 384 bits / 48 bytes
        { "4a16881ce156f45fdfdb45088e3f23be1b4c5a7a6a35315d36c51c75f275733319aca185d4ab33130ffe45f751f1bbc5", "a90d2aa5b241e1ca9dab5b6dc05c3e2c93fc5a2210a6315d60f9b791b36b560d70e135ef8e7dba9441b74e53dab0606b" },

        // 432 bits / 54 bytes
        { "a44c7f84ab962f68283404f8c5c4029dbc35d2138e075c9327580baf89f292937bf99422e45756b3f942bf0a5ae4acb6", "26bad23e51c4560c172076538b28716782ee6304962f68e27182048948d5c367a51a1c206a3e9b25135b40883b2e220f61cb5787ed8f" },

        // 616 bits / 77 bytes
        { "32286970204c3451958f5155f090448f061dd81b136a14592a3204c6b08e922ee5bb6d6534dbf8efb4bb7387092c8400", "357d5765595065efe281afb8d021d4764fba091adde05e02af0a437051a04a3b8e552ec48fb7152c470412c40e40eec58b842842d8993a5ae1c61eb20de5112321bc97af618bbfbaf8e2a87699" },

        // 704 bits / 88 bytes
        { "968ae9104f9c907c5a72936250dfedd62cd04f6e5ddd2c113490808a11884449aaef5d013ea3993a6cb6fc5c08754408", "9436da433d1ebd10b946b129cb34bccec9b8f705aaba3f8561352ed36a8449aba2dd7ba15b1bc308b0c02913163af63a346524dff5521432db477f529606afb5d552efc95cb040db566b4d39eddaa19319e518a7b5c6931e" },

        // 824 bits / 103 bytes
        { "b1319192df11faa00d3c4b068becc8f1ba3b00e0d1ff1f93c11a3663522fdb92ab3cca389634687c632e0a4b5a26ce92", "6c36147652e71b560becbca1e7656c81b4f70bece26321d5e55e67a3db9d89e26f2f2a38fd0f289bf7fa22c2877e38d9755412794cef24d7b855303c332e0cb5e01aa50bb74844f5e345108d6811d5010978038b699ffaa370de8473f0cda38b89a28ed6cabaf6" },

        // 832 bits / 104 bytes
        { "71307eec1355f73e5b726ed9efa1129086af81364e30a291f684dfade693cc4bc3d6ffcb7f3b4012a21976ff9edcab61", "92c41d34bd249c182ad4e18e3b856770766f1757209675020d4c1cf7b6f7686c8c1472678c7c412514e63eb9f5aee9f5c9d5cb8d8748ab7a5465059d9cbbb8a56211ff32d4aaa23a23c86ead916fe254cc6b2bff7a9553df1551b531f95bb41cbbc4acddbd372921" },

        // 1672 bits / 209 bytes
        { "3054d249f916a6039b2a9c3ebec1418791a0608a170e6d36486035e5f92635eaba98072a85373cb54e2ae3f982ce132b", "5fe35923b4e0af7dd24971812a58425519850a506dfa9b0d254795be785786c319a2567cbaa5e35bcf8fe83d943e23fa5169b73adc1fcf8b607084b15e6a013df147e46256e4e803ab75c110f77848136be7d806e8b2f868c16c3a90c14463407038cb7d9285079ef162c6a45cedf9c9f066375c969b5fcbcda37f02aacff4f31cded3767570885426bebd9eca877e44674e9ae2f0c24cdd0e7e1aaf1ff2fe7f80a1c4f5078eb34cd4f06fa94a2d1eab5806ca43fd0f06c60b63d5402b95c70c21ea65a151c5cfaf8262a46be3c722264b" },

        // 2512 bits / 314 bytes
        { "02535d86cc7518484a2a238c921b739b1704a50370a2924abf39958c5976e658dc5e87440063112459bddb40308b1c70", "035adcb639e5f28bb5c88658f45c1ce0be16e7dafe083b98d0ab45e8dcdbfa38e3234dfd973ba555b0cf8eea3c82ae1a3633fc565b7f2cc839876d3989f35731be371f60de140e3c916231ec780e5165bf5f25d3f67dc73a1c33655dfdf439dfbf1cbba8b779158a810ad7244f06ec078120cd18760af436a238941ce1e687880b5c879dc971a285a74ee85c6a746749a30159ee842e9b03f31d613dddd22975cd7fed06bd049d772cb6cc5a705faa734e87321dc8f2a4ea366a368a98bf06ee2b0b54ac3a3aeea637caebe70ad09ccda93cc06de95df73394a87ac9bbb5083a4d8a2458e91c7d5bf113aecae0ce279fdda76ba690787d26345e94c3edbc16a35c83c4d071b132dd81187bcd9961323011509c8f644a1c0a3f14ee40d7dd186f807f9edc7c02f6761061bbb6dd91a6c96ec0b9f10edbbd29dc52" },

        // 3352 bits / 419 bytes
        { "927962c873a69caa05cadc1cb485eb1cbb07748e47d942192df4af9233f42b95a638918306ae83a8237d21c2824f666d", "7f25b2c0eb1a6911cc3328fcdcd40f28f010375f7b1b51a05402896fb999b17093b59b34fb9cc653feba3dbb9d96bd47180416946d9bd3101b691d532be6ddb3712721121054c1fb3c5c42ee44e7faf7cf8d75856545187a3220047f07373e9aa2e10c022f2aa2320f81fd3cd7b110609c131edd6e016707228d069a55731a4ead4d24ab6206b01ffd91384e60db45a907fed7428db707de721aeb4c1b84baf61ad230b6b0d034eb90f4b9cbe64de2fb98b6695dcc4f4129aa2e7a3f635166bb72d7faca227076bd5013495c72ef2e7dd8a39cd532b15d0d53307c1834c265c53cc64890becfbebec454afa90ba973584e2d3752c7c6a3b4f48aba8297bf013b0006e3b08ed354157420b559b963f7b383bd047e94745a4615a3f9239230804547ff93d19a657fece8e02114840504b7fdb9c9fea0a4ccea3ee304a330fd2b0d97191f9be86e8968a9fabc847577e08b468b4f7df43f3fc9f8b2a2ab760f4ab87bbc51b883d4b8b33ed84e4f93a1d359e6995ea1962bfc0bca789ae36e4c25717850efcd708155f52fe09f1de76b2746634dbe1290524bd73d9db5f21f9d035e183dc2" },
    };

    for( auto const& input : inputs )
    {
        BOOST_TEST_EQ( digest<sha3_384>( from_hex( input.second ) ), std::string( input.first ));
    }
}

int main()
{
    sha3_224();
    sha3_256();
    sha3_384();
    sha3_512();

    return boost::report_errors();
}
