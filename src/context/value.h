#pragma once

#include <optional>
#include <variant>
#include <cstddef>

#include "ndarray/slice.hpp"
#include "ndarray/ndarray_ref.hpp"

#include "mpc/protocol.hpp"

#include "visibility.h"

namespace pppu
{

/// @class Value
/// @brief The main unit of calculation in PPPU, based on ndarray.
/// @param _Protocol Used encryption protocol
/// @param pdtype Domain of plain text
/// @param sdtype Domain of share text
template <typename _Protocol, typename pdtype, typename sdtype = mpc::convertProtocolPlainTypeToShareType<_Protocol, pdtype> >
class Value
{
    static_assert(
        mpc::isValidProtocolPlainSharePair< _Protocol, pdtype, sdtype >::value,
        "protocol plain and share type mismatch"
    );

  public:

    using PlainType = core::NDArrayRef<pdtype>;
    using ShareType = core::NDArrayRef<sdtype>;

  protected:

    std::variant<std::nullptr_t, PlainType, ShareType> _data;

    int64_t _fracbits;
    Visibility _vis;

    static constexpr std::size_t PlainIndex = 1;
    static constexpr std::size_t ShareIndex = 2;

  public:

    using Protocol = _Protocol;

    ~Value()                       = default;
    Value(Value&&)                 = default;
    Value(Value const&)            = default;
    Value& operator=(Value&&)      = default;
    Value& operator=(Value const&) = default;

    Value(): _fracbits{-1}, _vis{} {}

    /// @brief Get the fracbits of a Value object.
    /// @return The fracbits parameter of this Value object
    int64_t fracbits() const;

    /// @brief Set the fracbits of a Value object.
    /// @param fracbits The fracbits of a Value object which is an integer greater than or equal to 0
    /// @param force Set it to true to force a Value change its fracbits when it already have one 
    /// @return This Value object
    Value& set_fracbits(int64_t fracbits, bool force = false);

    /// @brief Get the visibility of a Value object.
    /// @return The visibility parameter of this Value object
    Visibility visibility() const;

    /// @brief Set the visibility of a Value object.
    /// @param vis The visibility of a Value object which is chosen from VIS_PUBLIC, VIS_PRIVATE and VIS_SHARE
    /// @param force Set it to true to force a Value change its visibility when it already have one 
    /// @return This Value object
    Value& set_visibility(Visibility vis, bool force = false);

    /// @brief Determine the visibility of value as public.
    /// @return If it is public, return true, otherwise return false
    bool is_public()   const { return _vis.is_public();  }

    /// @brief Determine the visibility of value as private.
    /// @return If it is private, return true, otherwise return false
    bool is_private()  const { return _vis.is_private(); }

    // bool is_share()    const { return _vis.is_share();   }

    /// @brief Get the owner of this Value.
    /// @return The owner's player id
    playerid_t owner() const { return _vis.owner(); }

    /// @brief Determine the visibility of value as public or private.
    /// @return If it is public or private, return true, otherwise return false
    bool is_plain() const { return _data.index() == PlainIndex; }

    /// @brief Determine the visibility of value as share.
    /// @return If it is share, return true, otherwise return false
    bool is_share() const { return _data.index() == ShareIndex; }

    /// @brief Assign the data of a Value with plain input.
    /// @param data Plain input
    Value& assign_p(PlainType const& data) { _data.template emplace<PlainIndex>(data); return *this; }

    /// @brief Assign the data of a Value with share input.
    /// @param data Share input
    Value& assign_s(ShareType const& data) { _data.template emplace<ShareIndex>(data); return *this; }

    /// @brief Assign the data of a Value with plain input.
    /// @param data Plain input
    Value& assign_p(PlainType&& data) { _data.template emplace<PlainIndex>(std::move(data)); return *this; }

    /// @brief Assign the data of a Value with share input.
    /// @param data Share input
    Value& assign_s(ShareType&& data) { _data.template emplace<ShareIndex>(std::move(data)); return *this; }

    /// @brief Get the plain data of a Value.
    /// @return Plain data of this Value
    PlainType& data_p() { return std::get<PlainIndex>(_data); } 

    /// @brief Get the share data of a Value.
    /// @return Share data of this Value
    ShareType& data_s() { return std::get<ShareIndex>(_data); }

    /// @brief Get the plain data of a Value.
    /// @return Plain data of this Value
    PlainType const& data_p() const { return std::get<PlainIndex>(_data); } 

    /// @brief Get the share data of a Value.
    /// @return Share data of this Value 
    ShareType const& data_s() const { return std::get<ShareIndex>(_data); }

    /// @brief Get the dimensions of the array in a Value object.
    /// @return The dimensions of the array in this Value object
    int64_t ndim() const;

    /// @brief Get the number of elements of the array in a Value object.
    /// @return The number of elements of the array in this Value object
    int64_t numel() const;

    /// @brief Get the offset of a Value object.
    /// @return The offset of this Value object
    int64_t offset() const;

    /// @brief Get the shape of a Value object.
    /// @note Only in one-dimensional arrays
    /// @return The shape of this Value object
    int64_t shape(int64_t dim) const;

    /// @brief Get the shape of a Value object.
    /// @note Only in multidimensional arrays
    /// @return The shape of this Value object
    std::vector<int64_t> const& shape() const;

    /// @brief Get the strides of a Value object.
    /// @return The strides of this Value object
    std::vector<int64_t> const& strides() const;

    /// @brief Used to copy a Value object.
    /// @return This Value object
    Value copy() const;

    /// @brief Used to get parts of a whole Value object.
    /// @return This Value object
    Value slice(std::vector<core::Slice> indicies) const;

    /// @brief Used to get parts of a whole Value object.
    /// @return This Value object
    Value slice(std::vector<std::variant<core::Slice, int64_t>> indicies) const;

    /// @brief Used to turn a Value from one shape into another.
    /// @return This Value object
    Value reshape(std::vector<int64_t> new_shape) const;

    /// @brief Used to do matrix transpose.
    /// @return This Value object
    Value transpose() const;

    /// @brief Used to broadcast the array in a Value object to a new shape.
    /// @return This Value object
    Value broadcast_to(std::vector<int64_t> new_shape) const;

    /// @brief Used to do multidimensional matrix permutation.
    /// @return This Value object
    Value permute(std::span<const int64_t> indicies) const;

    /// @brief Used to replace some contents in the array of a Value object.
    /// @return This Value object
    Value substitute(std::span<const int64_t> indicies, Value const& value) const;

    /// @brief Used to print the stored data of a Value object.
    /// @return The stored data of this Value object
    std::string to_string() const;

  protected:

    /// @brief Visit processed stored data by different means.
    /// @return Stored data
    template <typename Fn>
    requires std::same_as<
        std::invoke_result_t<Fn, PlainType>,
        std::invoke_result_t<Fn, ShareType>
    > decltype(auto) _visit1(Fn&& fn) const
    {
        if(this->is_plain())
            return std::invoke(std::forward<Fn>(fn), this->data_p() );
        else if(this->is_share())
            return std::invoke(std::forward<Fn>(fn), this->data_s() );
        else
            throw std::invalid_argument("bad data type");
    }

    /// @brief Visit processed stored data by different means and unify fracbits and visibility.
    /// @return Stored data
    template <typename Fn>
    requires (
        std::same_as< std::invoke_result_t<Fn, PlainType>, PlainType > &&
        std::same_as< std::invoke_result_t<Fn, ShareType>, ShareType >
    ) Value _visit2(Fn&& fn) const
    {
        Value ans;
        if( this->is_plain() )
            ans.assign_p( std::invoke( std::forward<Fn>(fn), this->data_p() ) )
                .set_fracbits( this->fracbits() )
                .set_visibility( this->visibility() );
        else if( this->is_share() )
            ans.assign_s( std::invoke( std::forward<Fn>(fn), this->data_s() ) )
                .set_fracbits( this->fracbits() )
                .set_visibility( this->visibility() );
        else
            throw std::invalid_argument("bad data type");
        return ans;
    }

};

/// @struct isValueImpl
/// @brief Determine whether it is an implementation of Value.
template <typename T>
struct isValueImpl: std::false_type {};

/// @struct isValueImpl
/// @brief Determine whether it is an implementation of Value.
template <typename _Protocol, typename pdtype, typename sdtype>
struct isValueImpl< Value<_Protocol, pdtype, sdtype> >: std::true_type {};

/// @struct isValueImpl
/// @brief Determine whether it is a Value.
template <typename T>
concept isValue = isValueImpl<T>::value;

} // namespace pppu
