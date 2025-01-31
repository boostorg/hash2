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

BOOST_FORCEINLINE BOOST_HASH2_SHA3_CONSTEXPR std::uint64_t read_lane( std::uint64_t state[ 25 ], int x, int y )
{
    return state[ 5 * y + x ];
}

BOOST_FORCEINLINE BOOST_HASH2_SHA3_CONSTEXPR void write_lane( std::uint64_t state[ 25 ], int x, int y, std::uint64_t lane )
{
    state[ 5 * y + x ] = lane;
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

BOOST_FORCEINLINE BOOST_HASH2_SHA3_CONSTEXPR void xor_lane( std::uint64_t state[ 25 ], int x, int y, std::uint64_t v )
{
    state[ 5 * y + x ] ^= v;
}

template<class = void>
struct keccak_constants
{
    // calculate these using Figure 2.4 in the Keccak reference with % 64 applied
    constexpr static int const rho_offsets[ 25 ] =
    {
        0, 1, 62, 28, 27,
        36, 44, 6, 55, 20,
        3, 10, 43, 25, 39,
        41, 45, 15, 21, 8,
        18, 2, 61, 56, 14,
    };

    // the actual ordering is the reverse of this list
    // but to keep the code simple, we use the fact that this operation is linear to apply it
    // in a different order so the indexing math is easier/more simple
    // otherwise, we run into a case where it's essentially touching the `0 - 1` index of the
    // array
    constexpr static int const pi_step[ 24 ] =
        { 1, 6, 9, 22, 14, 20, 2, 12, 13, 19, 23, 15, 4, 24, 21, 8, 16, 5, 3, 18, 17, 11, 7, 10 };


    constexpr static std::uint64_t const iota_rc[ 24 ] =
    {
        0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
        0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
        0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
        0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
        0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
        0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
        0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
        0x8000000000008080, 0x0000000080000001, 0x8000000080008008,
    };
};

inline BOOST_HASH2_SHA3_CONSTEXPR void keccak_permute( std::uint64_t state[ 25 ] )
{
    auto const num_rounds = 24;
    for( int i = 0; i < num_rounds; ++i )
    {
        {
            // theta

            std::uint64_t C1[ 5 ] = {};
            std::uint64_t C2[ 5 ] = {};

            for( int x = 0; x < 5; ++x )
            {
                C1[ x ] = state[ x ] ^ state[ x + 5 ] ^ state[ x + 10 ] ^ state[ x + 15 ] ^ state[ x + 20 ];
                C2[ x ] = detail::rotl( C1[ x ], 1 );
            }

            for( int x = 0; x < 5; ++x )
            {
                // in proper modulo math, (x - 1) % 5 is isomorphic to (x + 4 ) % 5
                for( int y = 0; y < 5; ++y )
                {
                    state[ 5 * y + x ] ^= C1[ ( x + 4 ) % 5] ^ C2[ ( x + 1 ) % 5 ];
                }
            }
        }

        {
            // rho and pi fused

            auto rho_offsets = keccak_constants<>::rho_offsets;
            auto pi_step = keccak_constants<>::pi_step;

            auto lane = detail::rotl( state[ 1 ], rho_offsets[ 1 ] );
            for( int t = 0; t < 23; ++t )
            {
                state[ pi_step[ t ] ] = detail::rotl( state[ pi_step[ t + 1 ] ], rho_offsets[ pi_step[ t + 1 ] ] );
            }
            state[ pi_step[ 23 ] ] = lane;
        }

        {
            // chi

            std::size_t const size_max = -1;

            for( int y = 0; y < 25; y += 5 )
            {
                auto C1 = state[ y ];
                auto C2 = state[ y + 1 ];
                for( int x = 0; x < 3; ++x )
                {
                    state[ y + x ] ^= ( state[ y + ( x + 1 ) ] ^ size_max ) & state[ y + ( x + 2 ) ];
                }
                state[ y + 3 ] ^= ( ~state[ y + 4 ] ) & C1;
                state[ y + 4 ] ^= ( ~C1 ) & C2;
            }
        }

        {
            // iota
            auto iota_rc = keccak_constants<>::iota_rc;
            state[ 0 ] ^= iota_rc[ i ];
        }
    }
}

} // namespace detail
} // namespace hash2
} // namespace boost


#endif // BOOST_HASH2_DETAIL_KECCAK_HPP_INCLUDED
