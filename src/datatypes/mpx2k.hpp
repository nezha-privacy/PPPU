#pragma once

#include <cmath>
#include <stdexcept>

#include "mpx2k.h"

#include "../tools/math.h"

namespace detail
{

/// @brief MP_BITS_PER_LIMB represents the number of bits in a single mp_limb_t data type.
constexpr size_t MP_BITS_PER_LIMB = 8 * sizeof(mp_limb_t);

/// TODO: conflict with mpxp
/// @brief N_LIMBS calculates the number of limbs required to represent K in bits.
///        N_LIMBS = (K + MP_BITS_PER_LIMB - 1) / MP_BITS_PER_LIMB. 
template <size_t K>
constexpr size_t N_LIMBS = ceildiv(K, MP_BITS_PER_LIMB);

/// @brief NORM_MASK indicates the normalized mask of the highest limbs.
template <size_t K>
constexpr mp_limb_t NORM_MASK = (~mp_limb_t(0)) >> (MP_BITS_PER_LIMB - 1 - ((K-1) % MP_BITS_PER_LIMB));

/// @brief Get the value of the bit at the specified position in the array of limbs.
/// @param sp Pointer to the source array containing the input value
/// @param pos Position of the bit to retrieve
/// @return The value of the bit at the specified position (0 or 1)
int bit(
    const mp_limb_t* sp,
    size_t pos
);

/// @brief Get the value of the bit at the most significant position in the array of limbs.
/// @param sp Pointer to the source array containing the input value
/// @return The value of the bit at the most significant position (0 or 1)
template <size_t K>
int most_significant_bit(
    const mp_limb_t* sp
) {
    static constexpr size_t limb_pos = (K-1) / MP_BITS_PER_LIMB;
    static constexpr size_t bit_pos  = (K-1) % MP_BITS_PER_LIMB;
    return 1 & (sp[limb_pos] >> bit_pos);
}

/// @brief Zero extension from a smaller size limb array to a larger size limb array.
/// @tparam destK is the bit length of the source array
/// @tparam srcK is the bit length of the destination array
/// @param rp Pointer to the array of limbs to be zeroed extension
template <size_t dest_K, size_t src_K> requires (dest_K > src_K)
void mpx2k_zero_extension(
    mp_limb_t* rp
) {
    if constexpr ( src_K % MP_BITS_PER_LIMB != 0 ) {
        static constexpr auto bitcnt = src_K % MP_BITS_PER_LIMB;
        static constexpr auto mask   = (mp_limb_t(1) << bitcnt) - 1;
        rp[N_LIMBS<src_K> - 1] &= mask; // set extra bits to zero
    }
    if constexpr ( N_LIMBS<dest_K> > N_LIMBS<src_K> ) {
        mpn_zero(rp + N_LIMBS<src_K>, N_LIMBS<dest_K> - N_LIMBS<src_K>);//set extra LIMBS to zero
    }
}

/// @brief Zero extension from a smaller size limb array to a larger size limb array.
/// @tparam destK is the bit length of the source array
/// @param srcK is the bit length of the destination array
/// @param rp Pointer to the array of limbs to be zeroed extension
template <size_t dest_K>
void mpx2k_zero_extension(
    mp_limb_t* rp,
    size_t src_K
) {
    auto src_limbs = ceildiv(src_K, MP_BITS_PER_LIMB);

    if ( src_K % MP_BITS_PER_LIMB != 0 ) {
        auto bitcnt = src_K % MP_BITS_PER_LIMB;
        auto mask   = (mp_limb_t(1) << bitcnt) - 1;
        rp[src_limbs - 1] &= mask; // set extra bits to zero
    }
    if ( N_LIMBS<dest_K> > src_limbs ) {
        mpn_zero(rp + src_limbs, N_LIMBS<dest_K> - src_limbs);//set extra LIMBS to zero
    }
}

/// @brief Signed extension from a smaller size limb array to a larger size limb array.
/// @tparam destK is the bit length of the source array
/// @tparam srcK is the bit length of the destination array
/// @param rp Pointer to the array of limbs to be signed extension
template <size_t dest_K, size_t src_K> requires (dest_K > src_K)
void mpx2k_sign_extension(
    mp_limb_t* rp
) {
    int msb = most_significant_bit<src_K>(rp);
    if(msb == 1) {
        if constexpr ( src_K % MP_BITS_PER_LIMB != 0 ) {
            static constexpr auto bitcnt = src_K % MP_BITS_PER_LIMB;
            static constexpr auto mask   = (mp_limb_t(1) << bitcnt) - 1;
            rp[N_LIMBS<src_K> - 1] |= ~mask; // set extra bits to one
        }
        if constexpr (  N_LIMBS<dest_K> > N_LIMBS<src_K> ) {
            static constexpr auto n_bytes = sizeof(mp_limb_t) * (N_LIMBS<dest_K> - N_LIMBS<src_K>);
            memset(rp + N_LIMBS<src_K>, 0xff, n_bytes); // set extra bits to one
        }
        mpx2k_norm<dest_K>(rp);
    }
    else /* msb == 0 */ {
        mpx2k_zero_extension<dest_K, src_K>(rp);
    }
}

/// @brief Signed extension from a smaller size limb array to a larger size limb array.
/// @tparam destK is the bit length of the source array
/// @param srcK is the bit length of the destination array
/// @param rp Pointer to the array of limbs to be signed extension
template <size_t dest_K>
void mpx2k_sign_extension(
    mp_limb_t* rp,
    size_t src_K
) {
    int msb = bit(rp, src_K - 1);
    auto src_limbs = ceildiv(src_K, MP_BITS_PER_LIMB);

    if(msb == 1) {
        if ( src_K % MP_BITS_PER_LIMB != 0 ) {
            auto bitcnt = src_K % MP_BITS_PER_LIMB;
            auto mask   = (mp_limb_t(1) << bitcnt) - 1;
            rp[src_limbs - 1] |= ~mask; // set extra bits to one
        }
        if (  N_LIMBS<dest_K> > src_limbs ) {
            auto n_bytes = sizeof(mp_limb_t) * (N_LIMBS<dest_K> - src_limbs);
            memset(rp + src_limbs, 0xff, n_bytes); // set extra bits to one
        }
        mpx2k_norm<dest_K>(rp);
    }
    else /* msb == 0 */ {
        mpx2k_zero_extension<dest_K>(rp, src_K);
    }
}

/// @brief value between [0, 2^K).
///        Set the extra bits in the highest limbs to 0
/// @param rp Pointer to the array of limbs to be normalized
template <size_t K>
void mpx2k_norm(mp_limb_t* rp) {
    if constexpr(K % MP_BITS_PER_LIMB != 0) {
        // Set the extra bits in the highest limbs to 0.
        rp[N_LIMBS<K> - 1] &= NORM_MASK<K>;
    }
}

/// @brief Set all bits to zero.
/// @param rp Pointer to the array of limbs to be zeroed out
template <size_t K>
void mpx2k_zero(mp_limb_t* rp) {
    mpn_zero(rp, N_LIMBS<K>);
}

/// @brief Set all bits to one.
/// @param rp Pointer to the array of limbs to be set all bits to one
template <size_t K>
void mpx2k_one(mp_limb_t* rp) {
    memset(rp, 0xff, N_LIMBS<K> * sizeof(mp_limb_t));
}

/// @brief rp = (-sp) mod 2k.
/// @param rp Pointer to the destination array for the result
/// @param sp Pointer to the source array containing the input value
template <size_t K>
void mpx2k_neg(
    mp_limb_t*       rp,
    const mp_limb_t* sp
) {
    mpn_neg(rp, sp, N_LIMBS<K>);
    mpx2k_norm<K>(rp);
}

/// @brief rp = (s1p + s2p) mod 2k.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
template <size_t K>
void mpx2k_add(
    mp_limb_t*       rp,
    const mp_limb_t* s1p,
    const mp_limb_t* s2p
) {
    mpn_add_n(rp, s1p, s2p, N_LIMBS<K>);
    mpx2k_norm<K>(rp);
}

/// @brief rp = (s1p - s2p) mod 2k.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
template <size_t K>
void mpx2k_sub(
    mp_limb_t*       rp,
    const mp_limb_t* s1p,
    const mp_limb_t* s2p
) {
    mpn_sub_n(rp, s1p, s2p, N_LIMBS<K>);
    mpx2k_norm<K>(rp);
}

/// @brief rp = (s1p * s2p) mod.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
template <size_t K>
void mpx2k_mul(
    mp_limb_t*       rp,
    const mp_limb_t* s1p,
    const mp_limb_t* s2p
) {
    // an informal test suggests that option1 is slightly faster
    constexpr size_t option = 1;

    if constexpr(option == 1) {
        static mp_limb_t buffer[2*N_LIMBS<K>];
        mpn_mul_n(buffer, s1p, s2p, N_LIMBS<K>);
        mpn_copyi(rp, buffer, N_LIMBS<K>);
    }
    else if constexpr (option == 2) {
        static mp_limb_t buffer[N_LIMBS<K>];
        mpn_zero(buffer, N_LIMBS<K>);
        for(size_t i = 0; i < N_LIMBS<K>; ++i) {
            mpn_addmul_1(buffer + i, s1p, N_LIMBS<K> - i, s2p[i]);
        }
        mpn_copyi(rp, buffer, N_LIMBS<K>);
    }
    else {
        throw std::runtime_error("unknown option for mpx2k_mul");
    }

    mpx2k_norm<K>(rp);
}

/// @brief rp = s1p & s2p.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
template <size_t K>
void mpx2k_and(
    mp_limb_t*       rp,
    const mp_limb_t* s1p,
    const mp_limb_t* s2p
) {
    mpn_and_n(rp, s1p, s2p, N_LIMBS<K>);
}

/// @brief rp = s1p | s2p.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
template <size_t K>
void mpx2k_ior(
    mp_limb_t*       rp,
    const mp_limb_t* s1p,
    const mp_limb_t* s2p
) {
    mpn_ior_n(rp, s1p, s2p, N_LIMBS<K>);
}

/// @brief rp = s1p ^ s2p.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
template <size_t K>
void mpx2k_xor(
    mp_limb_t*       rp,
    const mp_limb_t* s1p,
    const mp_limb_t* s2p
) {
    mpn_xor_n(rp, s1p, s2p, N_LIMBS<K>);
}

/// @brief rp = ~sp.
/// @param rp Pointer to the destination array for the result
/// @param sp Pointer to the source array containing the input value
template <size_t K>
void mpx2k_com(
    mp_limb_t*       rp,
    const mp_limb_t* sp
) {
    mpn_com(rp, sp, N_LIMBS<K>);
    mpx2k_norm<K>(rp);
}

/// @brief Copy the value of the array of limbs into a new array of limbs.
/// @param rp The new array of limbs
/// @param sp The copied array of limbs
/// @param n The size of the array of limbs
void mpx_copy(mp_limb_t* rp, const mp_limb_t* sp, mp_size_t n);

/// @brief rp = sp << cnt.
/// @param rp Pointer to the destination array for the result
/// @param sp Pointer to the source array containing the input value
/// @param cnt Left shift number, require cnt < K
template <size_t K>
void mpx2k_lshift(
    mp_limb_t*       rp,
    const mp_limb_t* sp,
    size_t           cnt
) {
    if(cnt >= K)
        throw std::invalid_argument("shift too much");

    size_t limbcnt = cnt / MP_BITS_PER_LIMB;
    size_t bitcnt  = cnt % MP_BITS_PER_LIMB;

    if(limbcnt == 0) {// cnt < MP_BITS_PER_LIMB 
        mpn_lshift(rp, sp, N_LIMBS<K>, bitcnt);
    } else {// cnt >= MP_BITS_PER_LIMB 
        mpx_copy(rp + limbcnt, sp, N_LIMBS<K> - limbcnt);
        mpn_zero(rp, limbcnt);
        mpn_lshift(rp + limbcnt, rp + limbcnt, N_LIMBS<K> - limbcnt, bitcnt);
    }

}

/// @brief rp = sp >> cnt.
/// @tparam Signed if Signed = true, Signed extension
///                if Signed = false, Unsigned extension
/// @param rp Pointer to the destination array for the result
/// @param sp Pointer to the source array containing the input value
/// @param cnt Right shift number, require cnt < K
template <size_t K, bool Signed>
void mpx2k_rshift(
    mp_limb_t*       rp,
    const mp_limb_t* sp,
    size_t           cnt
) {
    if(cnt >= K)
        throw std::invalid_argument("shift too much");

    size_t limbcnt = cnt / MP_BITS_PER_LIMB;
    size_t bitcnt  = cnt % MP_BITS_PER_LIMB;

    if(limbcnt == 0) {// cnt < MP_BITS_PER_LIMB
        mpn_rshift(rp, sp, N_LIMBS<K>, bitcnt);
    } else {// cnt >= MP_BITS_PER_LIMB
        mpx_copy(rp, sp + limbcnt, N_LIMBS<K> - limbcnt);
        mpn_rshift(rp, rp, N_LIMBS<K> - limbcnt, bitcnt);
    }

    if(Signed) {
        mpx2k_sign_extension<K>(rp, K-cnt);
    } else {
        mpx2k_zero_extension<K>(rp, K-cnt);
    }
}

/// @brief return s1p <=> s2p.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
/// @return if s1p < s2p, return a negative value
///         if s1p = s2p, return 0
///         if s1p > s2p, return a positive value
template <size_t K, bool Signed>
int mpx2k_cmp(
    const mp_limb_t* s1p,
    const mp_limb_t* s2p
) {
    if constexpr (Signed) {
        int sgn1 = most_significant_bit<K>(s1p);
        int sgn2 = most_significant_bit<K>(s2p);
        return (sgn1 != sgn2)
            ? (sgn2 -  sgn1) //Unequal sign
            : mpn_cmp(s1p, s2p, N_LIMBS<K>); //Equal sign
    } else {
        return mpn_cmp(s1p, s2p, N_LIMBS<K>);
    }
}

} // namespace detail
