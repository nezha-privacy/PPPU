#include "mpx2k.hpp"

namespace detail
{

/// @brief Get the value of the bit at the specified position in the array of limbs.
/// @param sp Pointer to the source array containing the input value
/// @param pos Position of the bit to retrieve
/// @return The value of the bit at the specified position (0 or 1)
int bit(
    const mp_limb_t* sp,
    size_t pos
) {
    size_t limb_pos = pos / mp_bits_per_limb;
    size_t bit_pos  = pos % mp_bits_per_limb;
    return 1 & (sp[limb_pos] >> bit_pos);
}

/// @brief Copy the value of the array of limbs into a new array of limbs.
/// @param rp The new array of limbs
/// @param sp The copied array of limbs
/// @param n The size of the array of limbs
void mpx_copy(mp_limb_t* rp, const mp_limb_t* sp, mp_size_t n) {

    if(rp == sp)  return;

    if(std::abs(sp - rp) >= n) {
        mpn_copyi(rp, sp, n);
    } else if( sp - rp > 0 ) {
        mpn_copyi(rp, sp, n);
    } else {
        mpn_copyd(rp, sp, n);
    }
}

} // namespace detail
