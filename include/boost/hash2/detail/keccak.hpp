#ifndef BOOST_HASH2_DETAIL_KECCAK_HPP_INCLUDED
#define BOOST_HASH2_DETAIL_KECCAK_HPP_INCLUDED

// Copyright 2025 Christian Mazakas
// Copyright 2025 Peter Dimov
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

BOOST_FORCEINLINE BOOST_HASH2_SHA3_CONSTEXPR void xor_lane( std::uint64_t state[ 25 ], int x, int y, std::uint64_t v )
{
    state[ 5 * y + x ] ^= v;
}

inline BOOST_HASH2_SHA3_CONSTEXPR void keccak_round( std::uint64_t (&state)[ 25 ] )
{
    {
        // theta

        std::uint64_t const C1[ 5 ] =
        {
            // state[ x ] ^ state[ x + 5 ] ^ state[ x + 10 ] ^ state[ x + 15 ] ^ state[ x + 20 ]

            state[ 0 ] ^ state[ 0 + 5 ] ^ state[ 0 + 10 ] ^ state[ 0 + 15 ] ^ state[ 0 + 20 ],
            state[ 1 ] ^ state[ 1 + 5 ] ^ state[ 1 + 10 ] ^ state[ 1 + 15 ] ^ state[ 1 + 20 ],
            state[ 2 ] ^ state[ 2 + 5 ] ^ state[ 2 + 10 ] ^ state[ 2 + 15 ] ^ state[ 2 + 20 ],
            state[ 3 ] ^ state[ 3 + 5 ] ^ state[ 3 + 10 ] ^ state[ 3 + 15 ] ^ state[ 3 + 20 ],
            state[ 4 ] ^ state[ 4 + 5 ] ^ state[ 4 + 10 ] ^ state[ 4 + 15 ] ^ state[ 4 + 20 ],
        };

        std::uint64_t const C2[ 5 ] =
        {
            // detail::rotl( C1[ x ], 1 )

            detail::rotl( C1[ 0 ], 1 ),
            detail::rotl( C1[ 1 ], 1 ),
            detail::rotl( C1[ 2 ], 1 ),
            detail::rotl( C1[ 3 ], 1 ),
            detail::rotl( C1[ 4 ], 1 ),
        };

        for( int y = 0; y < 5; ++y )
        {
            // for( int x = 0; x < 5; ++x )
            // {
            //     state[ 5 * y + x ] ^= C1[ ( x + 4 ) % 5] ^ C2[ ( x + 1 ) % 5 ];
            // }

            state[ 5 * y + 0 ] ^= C1[ 4 ] ^ C2[ 1 ];
            state[ 5 * y + 1 ] ^= C1[ 0 ] ^ C2[ 2 ];
            state[ 5 * y + 2 ] ^= C1[ 1 ] ^ C2[ 3 ];
            state[ 5 * y + 3 ] ^= C1[ 2 ] ^ C2[ 4 ];
            state[ 5 * y + 4 ] ^= C1[ 3 ] ^ C2[ 0 ];
        }
    }

    {
        // rho and pi fused

        // calculate these using Figure 2.4 in the Keccak reference with % 64 applied
        int const rho_offsets[ 25 ] =
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
        int const pi_step[ 24 ] =
            { 1, 6, 9, 22, 14, 20, 2, 12, 13, 19, 23, 15, 4, 24, 21, 8, 16, 5, 3, 18, 17, 11, 7, 10 };

        auto lane = detail::rotl( state[ 1 ], rho_offsets[ 1 ] );

        for( int t = 0; t < 23; ++t )
        {
            state[ pi_step[ t ] ] = detail::rotl( state[ pi_step[ t + 1 ] ], rho_offsets[ pi_step[ t + 1 ] ] );
        }

        state[ pi_step[ 23 ] ] = lane;
    }

    {
        // chi

        for( int y = 0; y < 5; ++y )
        {
            std::uint64_t plane[ 5 ] = {};

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
}

inline /*BOOST_HASH2_SHA3_CONSTEXPR*/ void keccak_permute( std::uint64_t (&state)[ 25 ] )
{
    static constexpr std::uint64_t iota_rc[ 24 ] =
    {
        0x0000000000000001ull, 0x0000000000008082ull, 0x800000000000808aull,
        0x8000000080008000ull, 0x000000000000808bull, 0x0000000080000001ull,
        0x8000000080008081ull, 0x8000000000008009ull, 0x000000000000008aull,
        0x0000000000000088ull, 0x0000000080008009ull, 0x000000008000000aull,
        0x000000008000808bull, 0x800000000000008bull, 0x8000000000008089ull,
        0x8000000000008003ull, 0x8000000000008002ull, 0x8000000000000080ull,
        0x000000000000800aull, 0x800000008000000aull, 0x8000000080008081ull,
        0x8000000000008080ull, 0x0000000080000001ull, 0x8000000080008008ull,
    };

    for( int i = 0; i < 24; ++i )
    {
        keccak_round( state );
        state[ 0 ] ^= iota_rc[ i ];
    }
}

} // namespace detail
} // namespace hash2
} // namespace boost


#endif // BOOST_HASH2_DETAIL_KECCAK_HPP_INCLUDED
