#pragma once

#include <stdexcept>

#include "Zp.h"

/// @brief Modulus of the mp_limb_t array representation.
template <size_t N>
mp_limb_t Zp<N>::_modulus[Zp<N>::N_LIMBS];

/// @brief Modulus of the mpz_class representation.
template <size_t N>
mpz_class Zp<N>::_modulus_class;

/// @brief Initializes the modulus of a finite field.
///        modulus is assigned to _modulus_class and _modulus[N_LIMBS].
/// @param modulus The mpz_class representation
template <size_t N>
void Zp<N>::init(mpz_class const& modulus) {
    
    // Check the sign of modulus
    if( modulus < 0 ) {
        throw std::invalid_argument("modulus must be positive");
    }

    // Check whether modulus is N bit in size
    if( mpz_sizeinbase( modulus.get_mpz_t(), 2 ) != N ) {
        throw std::invalid_argument("modulus must be exactly N bits");
    }

    // Check whether modulus is prime
    if( mpz_probab_prime_p( modulus.get_mpz_t(), 40 ) == 0 ) {
        throw std::invalid_argument("modulus is not a prime");
    }

    // Check whether modulus is prime
    _modulus_class = modulus;

    //Fill _modulus with word data from modulus.get_mpz_t().
    mpz_export(
        _modulus,
        nullptr,
        -1,
        sizeof(mp_limb_t),
        0, 0,
        modulus.get_mpz_t()
    );

}

/// @note All computation's correctness counts on correct input, thus proper initialization is required.
template <size_t N>
Zp<N>::Zp() { this->assign(0); }

/// @brief Constructs finite field elements using unsigned long integer values.
template <size_t N>
Zp<N>::Zp(unsigned long val) { this->assign(val); }

/// @brief Constructs finite field elements using mpz_class type values.
template <size_t N>
Zp<N>::Zp(mpz_class const& val) { this->assign(val); }

/// @brief Constructs finite field elements using strings and bases(default decimal).
template <size_t N>
Zp<N>::Zp(const std::string& str, int base) { this->assign(str, base); }

/// @brief Assigns a value of type unsigned long to a finite field element. 
template <size_t N>
void Zp<N>::assign(unsigned long val) {
    mpn_zero(_data, N_LIMBS);

    // Check whether val is greater than modulus
    if constexpr( N <= 64 ) {
        val %= _modulus[0];
    }

    _data[0] = val;
}

/// @brief Assigns a value of type mpz_class to a finite field element.
template <size_t N>
void Zp<N>::assign(mpz_class const& val) {

    // Check the sign of val
    bool negative  = (mpz_sgn(val.get_mpz_t()) < 0);
    // Check whether val is greater than modulus
    bool too_large = (mpz_cmp(val.get_mpz_t(), _modulus_class.get_mpz_t()) >= 0);

    mpz_t true_val;
    mpz_init(true_val);
    if( negative || too_large ) {
        mpz_mod(true_val, val.get_mpz_t(), _modulus_class.get_mpz_t());
    } else {
        mpz_set(true_val, val.get_mpz_t());
    }

    std::size_t count;
    //Fill _data with word data from true_val, the number of words produced is written to count.
    mpz_export(
        _data,
        &count,
        -1,
        sizeof(mp_limb_t),
        0, 0,
        true_val
    );

    if( count > N_LIMBS )  throw "write too much";
    // set extra bits to zero
    mpn_zero(_data + count, N_LIMBS - count);
}

/// @brief Assigns to finite field elements using strings and bases(default decimal).
template <size_t N>
void Zp<N>::assign(std::string const& str, int base) {
    mpz_class mpz_val(str, base);
    this->assign(mpz_val);
}

/// @brief Converts a finite field element to a string representation(default decimal).
template <size_t N>
std::string Zp<N>::to_string(int base) const {
    auto mpz_val = this->to_mpz();
    auto str = mpz_val.get_str(base);
    return str;
}

/// @brief Converts a finite field element to an mpz_class type representation.
template <size_t N>
mpz_class Zp<N>::to_mpz() const {
    mpz_class ans;

    // Set ans from an array of word data at _data.
    mpz_import(
        ans.get_mpz_t(),
        N_LIMBS,
        -1,
        sizeof(mp_limb_t),
        0, 0,
        _data
    );

    return ans;
}

template <size_t N>
Zp<N> Zp<N>::operator+(const Zp<N>& rhs) const {
    Zp<N> ans;
    detail::mpxp_add<N>(ans._data, _data, rhs._data, _modulus);
    return ans;
}

template <size_t N>
Zp<N> Zp<N>::operator-(const Zp<N>& rhs) const {
    Zp<N> ans;
    detail::mpxp_sub<N>(ans._data, _data, rhs._data, _modulus);
    return ans;
}

template <size_t N>
Zp<N> Zp<N>::operator*(const Zp<N>& rhs) const {
    Zp<N> ans;
    detail::mpxp_mul<N>(ans._data, _data, rhs._data, _modulus);
    return ans;
}

template <size_t N>
Zp<N> Zp<N>::operator/(const Zp<N>& rhs) const {
    Zp<N> ans;
    detail::mpxp_div<N>(ans._data, _data, rhs._data, _modulus);
    return ans;
}

template <size_t N>
Zp<N>& Zp<N>::operator+=(const Zp<N>& rhs) {
    detail::mpxp_add<N>(_data, _data, rhs._data, _modulus);
    return *this;
}

template <size_t N>
Zp<N>& Zp<N>::operator-=(const Zp<N>& rhs) {
    detail::mpxp_sub<N>(_data, _data, rhs._data, _modulus);
    return *this;
}

template <size_t N>
Zp<N>& Zp<N>::operator*=(const Zp<N>& rhs) {
    detail::mpxp_mul<N>(_data, _data, rhs._data, _modulus);
    return *this;
}

template <size_t N>
Zp<N>& Zp<N>::operator/=(const Zp<N>& rhs) {
    detail::mpxp_div<N>(_data, _data, rhs._data, _modulus);
    return *this;
}

template <size_t N>
bool Zp<N>::operator==(const Zp<N>& rhs) const {
    return detail::mpxp_cmp<N>(_data, rhs._data, _modulus) == 0;
}

/// @brief Compare this with input.
/// @return if *this < param, return std::strong_ordering::less
///         if *this = param, return std::strong_ordering::equal
///         if *this > param, return std::strong_ordering::greater
template <size_t N>
std::strong_ordering Zp<N>::operator<=>(const Zp<N>& rhs) const {
    auto cmp = detail::mpxp_cmp<N>(_data, rhs._data, _modulus);
    if(cmp < 0)        return std::strong_ordering::less;
    else if(cmp == 0)  return std::strong_ordering::equal;
    else               return std::strong_ordering::greater;
}
