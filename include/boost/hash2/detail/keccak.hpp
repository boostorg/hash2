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

BOOST_FORCEINLINE BOOST_HASH2_SHA3_CONSTEXPR void rho_and_pi_round( std::uint64_t (&state)[ 25 ], std::uint64_t& lane, int rot, int x, int y )
{
    std::uint64_t temp = state[ 5 * y + x ];
    state[ 5 * y + x ] = detail::rotl( lane, rot );
    lane = temp;
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

        std::uint64_t lane = state[ 1 ];

        rho_and_pi_round( state, lane,  1, 0, 2 );
        rho_and_pi_round( state, lane,  3, 2, 1 );
        rho_and_pi_round( state, lane,  6, 1, 2 );
        rho_and_pi_round( state, lane, 10, 2, 3 );
        rho_and_pi_round( state, lane, 15, 3, 3 );
        rho_and_pi_round( state, lane, 21, 3, 0 );
        rho_and_pi_round( state, lane, 28, 0, 1 );
        rho_and_pi_round( state, lane, 36, 1, 3 );
        rho_and_pi_round( state, lane, 45, 3, 1 );
        rho_and_pi_round( state, lane, 55, 1, 4 );
        rho_and_pi_round( state, lane,  2, 4, 4 );
        rho_and_pi_round( state, lane, 14, 4, 0 );
        rho_and_pi_round( state, lane, 27, 0, 3 );
        rho_and_pi_round( state, lane, 41, 3, 4 );
        rho_and_pi_round( state, lane, 56, 4, 3 );
        rho_and_pi_round( state, lane,  8, 3, 2 );
        rho_and_pi_round( state, lane, 25, 2, 2 );
        rho_and_pi_round( state, lane, 43, 2, 0 );
        rho_and_pi_round( state, lane, 62, 0, 4 );
        rho_and_pi_round( state, lane, 18, 4, 2 );
        rho_and_pi_round( state, lane, 39, 2, 4 );
        rho_and_pi_round( state, lane, 61, 4, 1 );
        rho_and_pi_round( state, lane, 20, 1, 1 );
        rho_and_pi_round( state, lane, 44, 1, 0 );
    }

    {
        // chi

        for( int y = 0; y < 5; ++y )
        {
            std::uint64_t const plane[ 5 ] =
            {
                state[ 5 * y + 0 ],
                state[ 5 * y + 1 ],
                state[ 5 * y + 2 ],
                state[ 5 * y + 3 ],
                state[ 5 * y + 4 ],
            };

            // state[ 5 * y + x ] = plane[ x ] ^ ( ( ~plane[ ( x + 1 ) % 5 ] ) & plane[ ( x + 2 ) % 5 ] );

            state[ 5 * y + 0 ] = plane[ 0 ] ^ ( ~plane[ 1 ] & plane[ 2 ] );
            state[ 5 * y + 1 ] = plane[ 1 ] ^ ( ~plane[ 2 ] & plane[ 3 ] );
            state[ 5 * y + 2 ] = plane[ 2 ] ^ ( ~plane[ 3 ] & plane[ 4 ] );
            state[ 5 * y + 3 ] = plane[ 3 ] ^ ( ~plane[ 4 ] & plane[ 0 ] );
            state[ 5 * y + 4 ] = plane[ 4 ] ^ ( ~plane[ 0 ] & plane[ 1 ] );
        }
    }
}

template<class = void> struct iota_rc_holder
{
    static constexpr std::uint64_t data[ 24 ] =
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
};

template<class T> constexpr std::uint64_t iota_rc_holder<T>::data[ 24 ];

inline BOOST_HASH2_SHA3_CONSTEXPR void keccak_permute( std::uint64_t (&state)[ 25 ] )
{
    for( int i = 0; i < 24; ++i )
    {
        keccak_round( state );
        state[ 0 ] ^= iota_rc_holder<>::data[ i ];
    }
}

} // namespace detail
} // namespace hash2
} // namespace boost


#endif // BOOST_HASH2_DETAIL_KECCAK_HPP_INCLUDED
