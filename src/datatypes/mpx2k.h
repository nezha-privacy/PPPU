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
/// @tparam K is bit length of the array

/// @brief Get the value of the bit at the specified position in the array of limbs.
/// @param sp Pointer to the source array containing the input value
/// @param pos Position of the bit to retrieve
/// @return The value of the bit at the specified position (0 or 1)
template <size_t K>
int bit(const mp_limb_t* sp, size_t pos);

/// @brief Get the value of the bit at the most significant position in the array of limbs.
/// @param sp Pointer to the source array containing the input value
/// @return The value of the bit at the most significant position (0 or 1)
template <size_t K>
int most_significant_bit(const mp_limb_t* sp);

/// @brief Zero extension from a smaller size limb array to a larger size limb array.
/// @tparam destK is the bit length of the source array
/// @tparam srcK is the bit length of the destination array
/// @param rp Pointer to the array of limbs to be zeroed extension
template <size_t destK, size_t srcK> requires (destK > srcK)
void mpx2k_zero_extension(mp_limb_t* rp);

/// @brief Signed extension from a smaller size limb array to a larger size limb array.
/// @tparam destK is the bit length of the source array
/// @tparam srcK is the bit length of the destination array
/// @param rp Pointer to the array of limbs to be signed extension
template <size_t destK, size_t srcK> requires (destK > srcK)
void mpx2k_sign_extension(mp_limb_t* rp);

/// @brief Zero extension from a smaller size limb array to a larger size limb array.
/// @tparam destK is the bit length of the source array
/// @param srcK is the bit length of the destination array
/// @param rp Pointer to the array of limbs to be zeroed extension
template <size_t destK>
void mpx2k_zero_extension(mp_limb_t* rp, size_t srcK);

/// @brief Signed extension from a smaller size limb array to a larger size limb array.
/// @tparam destK is the bit length of the source array
/// @param srcK is the bit length of the destination array
/// @param rp Pointer to the array of limbs to be signed extension
template <size_t destK>
void mpx2k_sign_extension(mp_limb_t* rp, size_t srcK);

/// @brief Value between [0, 2^K). Set the extra bits in the highest limbs to 0.
/// @param rp Pointer to the array of limbs to be normalized
template <size_t K>
void mpx2k_norm(mp_limb_t* rp);

/// @brief Set all bits to zero.
/// @param rp Pointer to the array of limbs to be zeroed out
template <size_t K>
void mpx2k_zero(mp_limb_t* rp);

/// @brief Set all bits to one.
/// @param rp Pointer to the array of limbs to be set all bits to one.
template <size_t K>
void mpx2k_one(mp_limb_t* rp);

/// @brief rp = (-sp) mod 2k.
/// @param rp Pointer to the destination array for the result
/// @param sp Pointer to the source array containing the input value
template <size_t K>
void mpx2k_neg(mp_limb_t* rp, const mp_limb_t* sp);

/// @brief rp = (s1p + s2p) mod 2k.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
template <size_t K>
void mpx2k_add(mp_limb_t* rp, const mp_limb_t* s1p, const mp_limb_t* s2p);

/// @brief rp = (s1p - s2p) mod 2k.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
template <size_t K>
void mpx2k_sub(mp_limb_t* rp, const mp_limb_t* s1p, const mp_limb_t* s2p);

/// @brief rp = (s1p * s2p) mod.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
template <size_t K>
void mpx2k_mul(mp_limb_t* rp, const mp_limb_t* s1p, const mp_limb_t* s2p);

/// @brief rp = (sp * sp) mod 2k.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
template <size_t K>
void mpx2k_sqr(mp_limb_t* rp, const mp_limb_t* s1p, const mp_limb_t* s2p);

/// @brief rp = s1p & s2p.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
template <size_t K>
void mpx2k_and(mp_limb_t* rp, const mp_limb_t* s1p, const mp_limb_t* s2p);

/// @brief rp = s1p | s2p.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
template <size_t K>
void mpx2k_ior(mp_limb_t* rp, const mp_limb_t* s1p, const mp_limb_t* s2p);

/// @brief rp = s1p ^ s2p.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
template <size_t K>
void mpx2k_xor(mp_limb_t* rp, const mp_limb_t* s1p, const mp_limb_t* s2p);

/// @brief rp = ~sp.
/// @param rp Pointer to the destination array for the result
/// @param sp Pointer to the source array containing the input value
template <size_t K>
void mpx2k_com(mp_limb_t* rp, const mp_limb_t* sp);

/// @brief rp = sp << cnt.
/// @param rp Pointer to the destination array for the result
/// @param sp Pointer to the source array containing the input value
/// @param cnt Left shift number, require cnt < K
template <size_t K>
void mpx2k_lshift(mp_limb_t* rp, const mp_limb_t* sp, size_t cnt);

/// @brief rp = sp >> cnt.
/// @tparam Signed if Signed = true, Signed extension
///                if Signed = false, Unsigned extension
/// @param rp Pointer to the destination array for the result
/// @param sp Pointer to the source array containing the input value
/// @param cnt Right shift number, require cnt < K
template <size_t K, bool Signed>
void mpx2k_rshift(mp_limb_t* rp, const mp_limb_t* sp, size_t cnt);

/// @brief Return s1p <=> s2p.
/// @param rp Pointer to the destination array for the result
/// @param s1p @param s2p PointerPointer to the source array containing the input value
/// @return if s1p < s2p, return a negative value
///         if s1p = s2p, return 0
///         if s1p > s2p, return a positive value
template <size_t K, bool Signed>
int mpx2k_cmp(const mp_limb_t* s1p, const mp_limb_t* s2p);

} // namespace detail
