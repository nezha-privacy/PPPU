#pragma once

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

#include <gmp.h>

#include "expr_template.h"
#include "raw_vector.hpp"
#include "bit_reference.h"

// expression templates eliminate temporary variables during expression evaluation
// compile with optimization -O1 at least
// about 2~3 times slower compared to using ^=, &=, |=
#define __bitvector_enable_expression_templates__

/// @class BitVector
/// @brief Support binary operations within bits per limb.
class BitVector
{
public:
    using size_type = std::size_t;

    static constexpr size_type N_BITS_PER_LIMB = GMP_LIMB_BITS;

protected:

    RawVector<mp_limb_t> _vec;
    size_type            _size;

public:
    using reference = BitReference<mp_limb_t>;

    ~BitVector() = default;

    BitVector();
    BitVector(BitVector&&);
    BitVector(BitVector const&) = delete;
    BitVector& operator=(BitVector&&);
    BitVector& operator=(BitVector const&) = delete;

    explicit BitVector(std::string const&);
    explicit BitVector(size_type);
    BitVector(size_type, bool);

    /// @brief Assign the value of this BitVector with a string form input like "010101".
    /// @param  The string form input
    void assign(std::string const&);

    /// @brief Print the BitVector to string.
    /// @return The string form of underlying data
    std::string to_string() const;

    /// @return The reference of element accessed
    reference  at        (size_type pos);
    bool       at        (size_type pos) const;
    reference  operator[](size_type pos);
    bool       operator[](size_type pos) const;

    /// @return The pointer of underlying data
    void*       data();
    const void* data() const;

    /// @brief Get the number of bytes needed to store bits.
    /// @return The number of bytes needed to store bits
    size_type size_in_bytes() const;

    /// @brief Get the number of limbs used to store bits.
    /// @return The number of limbs used to store bits
    size_type size_in_limbs() const;

    /// @brief Get the number of bits stored.
    /// @return The number of bits stored
    size_type size()          const;

    /// @brief Get the number of bits able to store.
    /// @return The number of bits able to store
    size_type capacity()      const;

    /// @brief Redefine the size of data has stored with n.
    /// @param  Input size of size_type
    void resize (size_type);

    /// @brief Open up the memory space required for the ByteVector of with specify size.
    /// @param  Input size of vector
    void reserve(size_type);

#ifdef __bitvector_enable_expression_templates__

    /// @brief Expression templates eliminate temporary variables during expression evaluation
    /// @tparam ExprType Expression type
    /// @param expr Expression
    template <ConceptExprCompound ExprType>
    BitVector(ExprType expr);

    /// @brief Expression templates eliminate temporary variables during expression evaluation
    /// @tparam ExprType Expression type
    /// @param expr Expression
    template <ConceptExprCompound ExprType>
    BitVector& operator=(ExprType expr);

#else

    BitVector  operator~() const;
    BitVector  operator^ (BitVector const&) const;
    BitVector  operator& (BitVector const&) const;
    BitVector  operator| (BitVector const&) const;

#endif

    void       invert();
    BitVector& operator^=(BitVector const&);
    BitVector& operator&=(BitVector const&);
    BitVector& operator|=(BitVector const&);

};
