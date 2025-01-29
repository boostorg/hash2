#ifndef BOOST_HASH2_DETAIL_KECCAK_HPP_INCLUDED
#define BOOST_HASH2_DETAIL_KECCAK_HPP_INCLUDED

// Copyright 2025 Christian Mazakas
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/hash2/detail/read.hpp>
#include <boost/hash2/detail/rot.hpp>
#include <boost/hash2/detail/write.hpp>
#include <boost/config.hpp>
#include <boost/config/workaround.hpp>

#include <cstdint>

#if BOOST_WORKAROUND(BOOST_GCC, >= 50000 && BOOST_GCC < 60000)
#define BOOST_HASH2_SHA3_CONSTEXPR
#else
#define BOOST_HASH2_SHA3_CONSTEXPR BOOST_CXX14_CONSTEXPR
#endif

namespace boost
{
namespace hash2
{
namespace detail
{

BOOST_FORCEINLINE BOOST_HASH2_SHA3_CONSTEXPR std::uint64_t read_lane( unsigned char state[ 200 ], int x, int y )
{
    return detail::read64le( &state[ 8 * (5 * y + x) ] );
}

BOOST_FORCEINLINE BOOST_HASH2_SHA3_CONSTEXPR void write_lane( unsigned char state[ 200 ], int x, int y, std::uint64_t lane )
{
    detail::write64le( &state[ 8 * ( 5 * y + x )], lane );
}

BOOST_FORCEINLINE BOOST_HASH2_SHA3_CONSTEXPR int lfsr(std::uint8_t& lfsr)
{
    int result = (lfsr & 0x01) != 0;
    if( lfsr & 0x80 ) {
        lfsr = ( ( lfsr << 1 ) ^ 0x71 );
    } else {
        lfsr <<= 1;
    }
    return result;
}

BOOST_FORCEINLINE BOOST_HASH2_SHA3_CONSTEXPR void xor_lane( unsigned char state[ 200 ], int x, int y, std::uint64_t v )
{
    auto p = &state[ 8 * ( 5 * y + x ) ];
    detail::write64le(p, detail::read64le(p) ^ v);
}

BOOST_FORCEINLINE BOOST_HASH2_SHA3_CONSTEXPR void keccak_permute( unsigned char state[ 200 ] )
{
    std::uint8_t lfsr_reg = 0x01;
    auto const num_rounds = 24;
    for( int i = 0; i < num_rounds; ++i )
    {
        {
            // theta

            std::uint64_t C[5] = {};

            for( int x = 0; x < 5; ++x )
            {
                C[ x ] = read_lane( state, x, 0 ) ^ read_lane( state, x, 1 ) ^ read_lane( state, x, 2 ) ^ read_lane( state, x, 3 ) ^ read_lane( state, x, 4 );
            }

            for( int x = 0; x < 5; ++x )
            {
                // in proper modulo math, (x - 1) % 5 is isomorphic to (x + 4 ) % 5
                std::uint64_t D = C[ ( x + 4) % 5 ] ^ detail::rotl( C[ ( x + 1 ) % 5 ], 1 );
                for( int y = 0; y < 5; ++y )
                {
                    xor_lane( state, x, y, D );
                }
            }
        }

        {
            // rho and pi fused

            /*
                The NIST publication might have a typo in its description of pi, which
                would yield incorrect offsets. The description claims:
                A'[x, y, z] = A[(x + 3y)%5,x,z]

                But the reference code and the Keccak reference use the correct rotation
                matrix in both functions, i.e.

                rho:
                |x| = |0 1| |x|
                |y|   |2 3| |y|

                expanded: x = 0 * x + 1 * y, y = 2 * x + 3 * y

                pi:
                |X| = |0 1| |x|
                |Y|   |2 3| |y|

                this expands to the same as above which makes the following assignment correct
            */

            int x = 1;
            int y = 0;

            std::uint64_t lane = read_lane( state, x, y );
            for( int t = 0; t < 24; ++t )
            {
                auto rot = ( ( t + 1 ) * ( t + 2 ) / 2 ) % 64;

                auto X = y;
                auto Y = ( 2 * x + 3 * y ) % 5;
                x = X;
                y = Y;

                auto temp = read_lane( state, x, y );
                write_lane( state, x, y, detail::rotl( lane, rot ) );
                lane = temp;
            }
        }

        {
            // chi

            std::uint64_t plane[ 5 ] = {};
            for( int y = 0; y < 5; ++y )
            {
                for( int x = 0; x < 5; ++x )
                {
                    plane[ x ] = read_lane( state, x, y );
                }

                for( int x = 0; x < 5; ++x )
                {
                    auto v = plane[ x ] ^ ( ( ~plane[ ( x + 1 ) % 5 ] ) & plane[ ( x + 2 ) % 5 ] );
                    write_lane( state, x, y, v );
                }
            }
        }

        {
            // iota
            for( int j = 0; j < 7; ++j )
            {
                unsigned pos = ( 1 << j ) - 1;
                if( lfsr( lfsr_reg ) )
                {
                    xor_lane( state, 0 ,0, ( std::uint64_t{1} << pos ));
                }
            }
        }
    }
}

} // namespace detail
} // namespace hash2
} // namespace boost


#endif // BOOST_HASH2_DETAIL_KECCAK_HPP_INCLUDED
