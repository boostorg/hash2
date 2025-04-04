// Copyright 2025 Christian Mazakas.
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//

#include <boost/hash2/blake2.hpp>

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

template<class H> std::string digest( std::string const& key, std::string const & s )
{
    std::string s1;
    std::string s2;

    {
        H h( key.data(), key.size() );

        h.update( s.data(), s.size() );
        s1 = to_string( h.result() );
    }

    {
        auto const n = s.size() / 3;

        H h( key.data(), key.size() );
        h.update( s.data(), n );
        h.update( s.data() + n , s.size() - n );
        s2 = to_string( h.result() );
    }

    BOOST_TEST_EQ( s1, s2 );

    return s1;
}

static void hmac_blake2b_512()
{
    using boost::hash2::hmac_blake2b_512;

    // selected samples from the download available here:
    // https://raw.githubusercontent.com/BLAKE2/BLAKE2/refs/heads/master/testvectors/blake2-kat.json
    // verified manually against openssl
    std::pair<char const*, char const*> inputs[] =
    {
          /* expected hash */                                                                                                                 /* input message */
        { "0035c4578a1cf2a0870bb8e93998c78b7fa498038ecd1b4a88f3dd5af9ad47ba77ba9b24e9b1af24347d7d3f13f90d957599133217df82e8d94471a24abce51b", "00" },
        { "65593d8ea75e25ab2ea9fbb6e00b902b8bd70b78ff486e322086682119e9d4c5a2c02152d6496ac379d27f9d1ce0475ac0cd2f6afcf5e3f0f3dc37b423cbb44e", "000102" },
        { "e70fb808369bd40bf3818308fdc77a5e33e73258cace9a219717ae6d38939e23b007cb0a5b9082507afd31522d5787ec6bb486f3924efa90f6efd2ec965533bb", "000102030405060708090a0b" },
        { "67e71127fa283ea8a2bf30478b798ae2410badaa22365dddc945227cec9a4125f64d8770ace008621f9212694fba5680f188b97f2d4995818659c2c52daa711c", "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e" },
        { "bbf22318eb88c87f482924502bf5e6217748322bcb709a834659aea90f56e338cbf11f38571c7fb60c555cc9d87b3f9d74e6b84d3d08e1dde2df5c5a7e1fe552", "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f" },
        { "bf34ad046462ff1066d89a82726f3ed3f7216cb719f3e147c514bd9b288d130c92a1f9fe2e288e09697d5ee78dbd0c196799c82847a87fdc5305474c44fa1a53", "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f80" },
        { "09aa4efcaf445acce347360276b7f65d63ae68676e60911d5182151842d7668512125f483dcafda56db4e357b41212c18870c22ca6688fa0fbc6cf9b24e4dc04", "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182" },
        { "b501e27503ffb6ba44a30b63d5d8ebf11d651b0dcadccfab29de70e5cdf23fac5498f30d9035facf96b0780f769688cb13ccb6e95a1fa70ad351ab2a87586c4a", "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a" },
        { "88d6768e921beb8d5f9b9819b57826ef93ffff0b20cdb4fdb1589ace551bbe96b71841678dc5fb58ef9719941287b27b30652b5103db1e257a4350ef1b182e7c", "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfe" },
    };

    std::string key = "abcdefghijklmnopqrstuvwxyz012345678900010203040506070809";

    for( auto const& input : inputs )
    {
        BOOST_TEST_EQ( digest<hmac_blake2b_512>( key, from_hex( input.second ) ), std::string( input.first ) );
    }
}

static void hmac_blake2s_256()
{
    using boost::hash2::hmac_blake2s_256;

    // selected samples from the download available here:
    // https://raw.githubusercontent.com/BLAKE2/BLAKE2/refs/heads/master/testvectors/blake2-kat.json
    // verified manually against openssl
    std::pair<char const*, char const*> inputs[] =
    {
          /* expected hash */                                                 /* input message */
        { "6e8d76e8f291316bdd50db9e17f26a312b12484b389c820fac0e961c31b25e6a", "00" },
        { "cc09f3f203cf4f421895ff7b9a4bfc0a551d365f71615ab98bcbbf8213067fe1", "000102" },
        { "3d15d6be16a4654608ec18791f59be980eb0368089ac67da7f37236a53eb727d", "000102030405060708090a0b" },
        { "7dc1cfef2b6602a5de1ecfb0b2e4c4981ff32d0c3205cbae570dc0cea98c5eac", "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f" },
        { "8459bd24f9568e8727572397aca2aa3c64599f831e24005453731bcedaf2fbe7", "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e" },
        { "7e4802915642b7cedc6e0682b2cce56a6a937f19bb121fd049c73ed09a6886f6", "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f" },
        { "fd5d2dac14fcacdc04c2867431d23295e742a9114a5c3034ad695df67d0a4d6f", "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f80" },
        { "e71051783f295e70660ee90fac25bf9169bcb45fd95a8ac7b535348b9cc18425", "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182" },
        { "77ab7d1d122cd58fb93f587fe79549abd64799af5f43b7f7694577d9f548d193", "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a" },
        { "8bb56272ab32589f8dcce72d1330823a6ad5c009dd1e48d3df3ddd3bd0c0e1b4", "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfe" },
    };

    std::string key = "abcdefghijklmnopqrstuvwxyz012345678900010203040506070809";

    for( auto const& input : inputs )
    {
        BOOST_TEST_EQ( digest<hmac_blake2s_256>( key, from_hex( input.second ) ), std::string( input.first ) );
    }
}

int main()
{
    hmac_blake2b_512();
    hmac_blake2s_256();

    return boost::report_errors();
}
