#include "bit_vector.hpp"

#include "../tools/math.h"

using size_type = BitVector::size_type;

/************************ constructor ************************/

BitVector::BitVector()
    : _size(0)
{
}

BitVector::BitVector(BitVector &&other)
{
    _size = other._size;
    _vec = std::move(other._vec);
    other._size = 0;
}

BitVector &BitVector::operator=(BitVector &&other)
{
    _size = other._size;
    _vec = std::move(other._vec);
    other._size = 0;
    return *this;
}

BitVector::BitVector(size_type n)
    : _size(n), _vec(ceildiv(n, N_BITS_PER_LIMB))
{
}

BitVector::BitVector(size_type n, bool val)
    : _size(n), _vec(ceildiv(n, N_BITS_PER_LIMB))
{
    auto ch = (val ? 0xff : 0x00);
    memset(this->data(), ch, this->size_in_bytes());
}

BitVector::BitVector(std::string const &str)
    : _size(0)
{
    this->assign(str);
}

/************************ assign ************************/

/// @brief Assign the value of this BitVector with a string form input like "010101".
/// @param str The string form input
void BitVector::assign(std::string const &str)
{
    size_type n = str.size();
    this->resize(n);
    for (size_type i = 0; i < n; ++i) {
        if (str[i] == '0')
            this->at(i) = false;
        else if (str[i] == '1')
            this->at(i) = true;
        else
            throw std::invalid_argument("invalid character for bit string");
    }
}

/// @brief Print the BitVector to string.
/// @return The string form of underlying data
std::string BitVector::to_string() const
{
    std::string str;
    size_type n = this->size();
    for (size_type i = 0; i < n; ++i) {
        str += ('0' + this->at(i));
    }
    return str;
}

/************************ capacity ************************/

/// @brief Get the number of bytes needed to store bits.
/// @return The number of bytes needed to store bits
size_type BitVector::size_in_bytes() const
{
    return ceildiv(_size, 8);
}

/// @brief Get the number of limbs used to store bits.
/// @return The number of limbs used to store bits
size_type BitVector::size_in_limbs() const
{
    return _vec.size();
}

/// @brief Get the number of bits stored.
/// @return The number of bits stored
size_type BitVector::size() const
{
    return _size;
}

/// @brief Get the number of bits able to store.
/// @return The number of bits able to store
size_type BitVector::capacity() const
{
    return _vec.capacity() * N_BITS_PER_LIMB;
}

/// @brief Redefine the size of data has stored with n.
/// @param n Input size of size_type
void BitVector::resize(size_type n)
{
    size_type n_limbs = ceildiv(n, N_BITS_PER_LIMB);
    _vec.resize(n_limbs);
    _size = n;
}

/// @brief Open up the memory space required for the ByteVector of with specify size.
/// @param n Input size of vector
void BitVector::reserve(size_type n)
{
    size_type n_limbs = ceildiv(n, N_BITS_PER_LIMB);
    _vec.reserve(n_limbs);
}

/************************ data access ************************/

/// @return The reference of element accessed
BitVector::reference BitVector::at(size_type pos)
{
    if (pos > _size)
        throw std::out_of_range("");

    return this->operator[](pos);
}

BitVector::reference BitVector::operator[](size_type pos)
{
    size_type limb_idx = pos / N_BITS_PER_LIMB;
    size_type bit_idx = pos % N_BITS_PER_LIMB;
    auto ptr = reinterpret_cast<mp_limb_t *>(this->data());
    return reference(ptr + limb_idx, bit_idx);
}

bool BitVector::at(size_type pos) const
{
    if (pos > _size)
        throw std::out_of_range("");

    return this->operator[](pos);
}

bool BitVector::operator[](size_type pos) const
{
    size_type limb_idx = pos / N_BITS_PER_LIMB;
    size_type bit_idx = pos % N_BITS_PER_LIMB;
    auto ptr = reinterpret_cast<mp_limb_t const *>(this->data());
    return 1 & (ptr[limb_idx] >> bit_idx);
}

/// @return The pointer of underlying data
void *BitVector::data()
{
    return _vec.data();
}

const void *BitVector::data() const
{
    return _vec.data();
}

/************************ binary opertation ************************/

#ifndef __bitvector_enable_expression_templates__

BitVector BitVector::operator~() const
{
    BitVector ans(_size);
    mpn_com(
        (mp_limb_t *)ans.data(), (mp_limb_t *)this->data(),
        this->size_in_limbs());
    return ans;
}

BitVector BitVector::operator^(BitVector const &other) const
{
    if (this->size() != other.size())
        throw std::invalid_argument("bitvector size mismatch");

    BitVector ans(_size);
    mpn_xor_n(
        (mp_limb_t *)ans.data(), (mp_limb_t *)this->data(),
        (mp_limb_t *)other.data(), this->size_in_limbs());
    return ans;
}

BitVector BitVector::operator&(BitVector const &other) const
{
    if (_size != other._size)
        throw std::invalid_argument("bitvector size mismatch");

    BitVector ans(_size);
    mpn_and_n(
        (mp_limb_t *)ans.data(), (mp_limb_t *)this->data(),
        (mp_limb_t *)other.data(), this->size_in_limbs());
    return ans;
}

BitVector BitVector::operator|(BitVector const &other) const
{
    if (_size != other._size)
        throw std::invalid_argument("bitvector size mismatch");

    BitVector ans(_size);
    mpn_ior_n(
        (mp_limb_t *)ans.data(), (mp_limb_t *)this->data(),
        (mp_limb_t *)other.data(), this->size_in_limbs());
    return ans;
}

void BitVector::invert()
{
    mpn_com((mp_limb_t *)this->data(), (mp_limb_t *)this->data(), this->size_in_limbs());
}

#endif // #ifdef __bitvector_enable_expression_templates__

BitVector &BitVector::operator^=(BitVector const &other)
{
    if (_size != other._size)
        throw std::invalid_argument("bitvector size mismatch");

    mpn_xor_n(
        (mp_limb_t *)this->data(), (mp_limb_t *)this->data(),
        (mp_limb_t *)other.data(), this->size_in_limbs());
    return *this;
}

BitVector &BitVector::operator&=(BitVector const &other)
{
    if (_size != other._size)
        throw std::invalid_argument("bitvector size mismatch");

    mpn_and_n(
        (mp_limb_t *)this->data(), (mp_limb_t *)this->data(),
        (mp_limb_t *)other.data(), this->size_in_limbs());
    return *this;
}

BitVector &BitVector::operator|=(BitVector const &other)
{
    if (_size != other._size)
        throw std::invalid_argument("bitvector size mismatch");

    mpn_ior_n(
        (mp_limb_t *)this->data(), (mp_limb_t *)this->data(),
        (mp_limb_t *)other.data(), this->size_in_limbs());
    return *this;
}
