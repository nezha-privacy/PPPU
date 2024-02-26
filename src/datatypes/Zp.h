#pragma once

#include <cstdlib>
#include <cstdint>
#include <compare>

#include <gmp.h>
#include <gmpxx.h>

#include "mpxp.hpp"

/// @class Zp
/// @brief The 'Zp' class represents a finite field whose modulus is prime,
///        and provides basic operations and operations on finite fields.
/// @tparam N_BITS is a number of bits to represent the modulus p.
template <size_t N_BITS>
class Zp {

protected:

    /// @details For an array of type mp_limb_t, the least significant byte 
    ///          is always in front and the native byte order of the host CPU is used.

    /// @brief N_LIMBS calculates the number of limbs required to represent N_BITS in bits.
    static constexpr std::size_t N_LIMBS = detail::ZP_LIMBS<N_BITS>;

    /// @brief Modulus of the mp_limb_t array representation.
    static mp_limb_t _modulus[N_LIMBS];
    /// @brief Modulus of the mpz_class representation.
    static mpz_class _modulus_class;

    /// @brief A mp_limb_t array that stores finite field elements.
    /// @note only positive number, which is different from Z2k.
    mp_limb_t _data[N_LIMBS];

public:
    static constexpr bool trivially_serializable = true;

    /// @brief Initializes the modulus of a finite field.
    ///        modulus is assigned to _modulus_class and _modulus[N_LIMBS].
    /// @param modulus The mpz_class representation
    static void init(mpz_class const& modulus);

    ~Zp()                    = default;
    Zp(Zp&&)                 = default;
    Zp(const Zp&)            = default;
    Zp& operator=(Zp&&)      = default;
    Zp& operator=(const Zp&) = default;

    /// @note All computation's correctness counts on correct input, thus proper initialization is required.
    Zp();
    /// @brief Constructs finite field elements using unsigned long integer values.
    Zp(unsigned long);
    /// @brief Constructs finite field elements using mpz_class type values.
    explicit Zp(mpz_class const&);
    /// @brief Constructs finite field elements using strings and bases(default decimal).
    explicit Zp(std::string const&, int base = 10);
    
    /// @brief Converts a finite field element to a string representation(default decimal).
    std::string to_string(int base = 10) const;
    /// @brief Converts a finite field element to an mpz_class type representation.
    mpz_class   to_mpz() const;
    
    /// @brief Assigns a value of type unsigned long to a finite field element. 
    void assign(unsigned long);
    /// @brief Assigns a value of type mpz_class to a finite field element.
    void assign(mpz_class const&);
    /// @brief Assigns to finite field elements using strings and bases(default decimal).
    void assign(std::string const&, int base);

    Zp  operator+ (const Zp&) const;
    Zp  operator- (const Zp&) const;
    Zp  operator* (const Zp&) const;
    Zp  operator/ (const Zp&) const;

    Zp& operator+=(const Zp&);
    Zp& operator-=(const Zp&);
    Zp& operator*=(const Zp&);
    Zp& operator/=(const Zp&);

    bool operator==(const Zp&) const;
    /// @brief Compare this with input.
    /// @return if *this < param, return std::strong_ordering::less
    ///         if *this = param, return std::strong_ordering::equal
    ///         if *this > param, return std::strong_ordering::greater
    std::strong_ordering operator<=>(const Zp&) const;

};
