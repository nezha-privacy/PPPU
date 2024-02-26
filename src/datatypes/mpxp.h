#pragma once

#include <gmp.h>

namespace detail
{
/// @note mp_limb_t is a data type commonly used in the GNU Multiple Precision Arithmetic Library (GMP).
///       It is an unsigned integer type used to represent bits of large integers.
///       In the GMP library, large integers are decomposed into multiple bits of type mp_limb_t.
///       The size of mp_limb_t is usually the size of a machine word.
/// @details For an array of type mp_limb_t, the least significant byte 
///          is always in front and the native byte order of the host CPU is used.
/// @tparam N the length of mp_limb_t array.

/// @brief rp = (-sp) mod pp.
/// @param rp Pointer to the destination array for the result
/// @param sp Pointer to the source array containing the input value
/// @param pp Pointer to the source array containing the modulus
template <size_t N>
void mpxp_neg(mp_limb_t* rp, const mp_limb_t* sp, const mp_limb_t* pp);

/// @brief rp = (s1p + s2p) mod pp.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
/// @param pp Pointer to the source array containing the modulus
template <size_t N>
void mpxp_add(mp_limb_t* rp, const mp_limb_t* s1p, const mp_limb_t* s2p, const mp_limb_t* pp);

/// @brief rp = (s1p - s2p) mod pp.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
/// @param pp Pointer to the source array containing the modulus
template <size_t N>
void mpxp_sub(mp_limb_t* rp, const mp_limb_t* s1p, const mp_limb_t* s2p, const mp_limb_t* pp);

/// @brief rp = (s1p * s2p) mod pp.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
/// @param pp Pointer to the source array containing the modulus
template <size_t N>
void mpxp_mul(mp_limb_t* rp, const mp_limb_t* s1p, const mp_limb_t* s2p, const mp_limb_t* pp);

/// @brief rp = (sp * sp) mod pp.
/// @param rp Pointer to the destination array for the result
/// @param sp Pointer to the source array containing the input value
/// @param pp Pointer to the source array containing the modulus
template <size_t N>
void mpxp_sqr(mp_limb_t* rp, const mp_limb_t* sp, const mp_limb_t* pp);

/// @brief rp = (1 / sp) mod pp.
/// @param rp Pointer to the destination array for the result
/// @param sp Pointer to the source array containing the input value
/// @param pp Pointer to the source array containing the modulus
template <size_t N>
void mpxp_inv(mp_limb_t* rp, const mp_limb_t* sp, const mp_limb_t* pp);

/// @brief rp = (s1p / s2p) mod pp.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
/// @param pp Pointer to the source array containing the modulus
template <size_t N>
void mpxp_div(mp_limb_t* rp, const mp_limb_t* s1p, const mp_limb_t* s2p, const mp_limb_t* pp);

/// @brief return s1p <=> s2p.
/// @param s1p @param s2p PointerPointer to the source array containing the input value
/// @param pp Pointer to the source array containing the modulus
/// @return if s1p < s2p, return a negative value
///         if s1p = s2p, return 0
///         if s1p > s2p, return a positive value
template <size_t N>
int mpxp_cmp(const mp_limb_t* s1p, const mp_limb_t* s2p, const mp_limb_t* pp);

} // namespace detail
