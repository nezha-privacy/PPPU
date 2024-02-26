#pragma once

#include "value.h"

namespace pppu
{

/// @brief Set the visibility of a Value object.
/// @param vis The visibility of a Value object which is chosen from VIS_PUBLIC, VIS_PRIVATE and VIS_SHARE
/// @param force Set it to true to force a Value change its visibility when it already have one 
/// @return This Value object
template <typename Protocol, typename pdtype, typename sdtype>
Value<Protocol, pdtype, sdtype>& Value<Protocol, pdtype, sdtype>::set_visibility(Visibility vis, bool force) {
    if(this->_vis.is_invalid() || force == true) {
        this->_vis = vis;
    }
    else {
        throw std::runtime_error("unable to set visibiilty");
    }
    return *this;
}

/// @brief Set the fracbits of a Value object.
/// @param fracbits The fracbits of a Value object which is an integer greater than or equal to 0
/// @param force Set it to true to force a Value change its fracbits when it already have one 
/// @return This Value object
template <typename Protocol, typename pdtype, typename sdtype>
Value<Protocol, pdtype, sdtype>& Value<Protocol, pdtype, sdtype>::set_fracbits(int64_t fracbits, bool force) {
    if(this->_fracbits == -1 || force == true) {
        this->_fracbits = fracbits;
    }
    else {
        throw std::runtime_error("unable to set fracbits");
    }
    return *this;
}

/// @brief Get the visibility of a Value object.
/// @return The visibility parameter of this Value object
template <typename Protocol, typename pdtype, typename sdtype>
Visibility Value<Protocol, pdtype, sdtype>::visibility() const
{
    if(this->_vis.is_invalid()) {
        throw std::runtime_error("visibility used before initialized");
    }

    return this->_vis;
}

/// @brief Get the fracbits of a Value object.
/// @return The fracbits parameter of this Value object
template <typename Protocol, typename pdtype, typename sdtype>
int64_t Value<Protocol, pdtype, sdtype>::fracbits() const
{
    if(this->_fracbits == -1) {
        throw std::runtime_error("fracbits used before initialized");
    }

    return this->_fracbits;
}

/// @brief Used to print the stored data of a Value object.
/// @return The stored data of this Value object
template <typename Protocol, typename pdtype, typename sdtype>
std::string Value<Protocol, pdtype, sdtype>::to_string() const
{
    return _visit1([](auto const& x){ return x.to_string(); });
}

/// @brief Get the dimensions of the array in a Value object.
/// @return The dimensions of the array in this Value object
template <typename Protocol, typename pdtype, typename sdtype>
int64_t Value<Protocol, pdtype, sdtype>::ndim() const
{
    return _visit1([](auto const& x){return x.ndim();});
}

/// @brief Get the number of elements of the array in a Value object.
/// @return The number of elements of the array in this Value object
template <typename Protocol, typename pdtype, typename sdtype>
int64_t Value<Protocol, pdtype, sdtype>::numel() const
{
    return _visit1([](auto const& x){return x.numel();});
}

/// @brief Get the shape of a Value object.
/// @return The shape of this Value object
template <typename Protocol, typename pdtype, typename sdtype>
int64_t Value<Protocol, pdtype, sdtype>::shape(int64_t dim) const
{
    return _visit1([dim](auto const& x){return x.shape(dim);});
}

/// @brief Get the shape of a Value object.
/// @return The shape of this Value object
template <typename Protocol, typename pdtype, typename sdtype>
std::vector<int64_t> const& Value<Protocol, pdtype, sdtype>::shape() const
{
    return _visit1([](auto const& x)-> std::vector<int64_t> const& {return x.shape();});
}

/// @brief Get the strides of a Value object.
/// @return The strides of this Value object
template <typename Protocol, typename pdtype, typename sdtype>
std::vector<int64_t> const& Value<Protocol, pdtype, sdtype>::strides() const
{
    return _visit1([](auto const& x)-> std::vector<int64_t> const& {return x.strides();});
}

/// @brief Get the offset of a Value object.
/// @return The offset of this Value object
template <typename Protocol, typename pdtype, typename sdtype>
int64_t Value<Protocol, pdtype, sdtype>::offset() const
{
    return _visit1([](auto const& x){return x.offset();});
}

/// @brief Used to turn a Value from one shape into another.
/// @return This Value object
template <typename Protocol, typename pdtype, typename sdtype>
Value<Protocol, pdtype, sdtype> Value<Protocol, pdtype, sdtype>::reshape(std::vector<int64_t> new_shape) const
{
    return _visit2([&new_shape](auto const& x){return x.reshape(std::move(new_shape));});
}

/// @brief Used to get parts of a whole Value object.
/// @return This Value object
template <typename Protocol, typename pdtype, typename sdtype>
Value<Protocol, pdtype, sdtype> Value<Protocol, pdtype, sdtype>::slice(std::vector<core::Slice> indicies) const
{
    return _visit2([&indicies](auto const& x){ return x.slice(indicies); });
}

/// @brief Used to get parts of a whole Value object.
/// @return This Value object
template <typename Protocol, typename pdtype, typename sdtype>
Value<Protocol, pdtype, sdtype> Value<Protocol, pdtype, sdtype>::slice(std::vector<std::variant<core::Slice, int64_t>> indicies) const
{
    return _visit2([&indicies](auto const& x){ return x.slice(indicies); });
}

/// @brief Used to do matrix transpose.
/// @return This Value object
template <typename Protocol, typename pdtype, typename sdtype>
Value<Protocol, pdtype, sdtype> Value<Protocol, pdtype, sdtype>::transpose() const
{
    return _visit2([](auto const& x){ return x.transpose(); });
}

/// @brief Used to broadcast the array in a Value object to a new shape.
/// @return This Value object
template <typename Protocol, typename pdtype, typename sdtype>
Value<Protocol, pdtype, sdtype> Value<Protocol, pdtype, sdtype>::broadcast_to(std::vector<int64_t> new_shape) const
{
    return _visit2([&new_shape](auto const& x){ return x.broadcast_to( std::move(new_shape) ); });
}

/// @brief Used to copy a Value object.
/// @return This Value object
template <typename Protocol, typename pdtype, typename sdtype>
Value<Protocol, pdtype, sdtype> Value<Protocol, pdtype, sdtype>::copy() const
{
    return _visit2([](auto const& x){ return x.copy(); });
}

/// @brief Used to do multidimensional matrix permutation.
/// @return This Value object
template <typename Protocol, typename pdtype, typename sdtype>
Value<Protocol, pdtype, sdtype> Value<Protocol, pdtype, sdtype>::permute(std::span<const int64_t> indicies) const
{
    return _visit2([indicies](auto const& x){ return x.permute(indicies); });
}

/// @brief Used to replace some contents in the array of a Value object.
/// @return This Value object
template <typename Protocol, typename pdtype, typename sdtype>
Value<Protocol, pdtype, sdtype> Value<Protocol, pdtype, sdtype>::substitute(
    std::span<const int64_t> indicies,
    Value<Protocol, pdtype, sdtype> const& value
) const {

    if(value.is_plain()) {
        return _visit2([indicies, data = value.data_p()](auto const& x){ return x.substitute(indicies, data); });
    }
    else if(value.is_share()) {
        return _visit2([indicies, data = value.data_s()](auto const& x){ return x.substitute(indicies, data); });
    }
    else {
        throw std::runtime_error("unknown visibility type");
    }
}

} // namespace pppu
