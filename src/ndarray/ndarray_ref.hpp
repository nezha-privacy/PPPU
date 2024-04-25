#pragma once

#include "ndarray_ref.h"

#include <ranges>

namespace core
{
/************************ element access ************************/

/// @brief Return a reference to the element at the specified index location.
/// @param raw.size() == shape.size()
/// @return Reference to the element at the specified index location
template <typename dtype>
NDArrayRef<dtype>::reference NDArrayRef<dtype>::elem(std::vector<int64_t> const &raw)
{
    auto index = detail::normalize(raw, _shape);
    int64_t pos = detail::calcNDIndex(index, _strides, _offset, this->ndim());
    return _buffer->operator[](pos);
}

/// @brief Return a reference to the element at the specified index location.
/// @param raw.size() == shape.size()
/// @return Reference to the element at the specified index location
template <typename dtype>
NDArrayRef<dtype>::const_reference NDArrayRef<dtype>::elem(std::vector<int64_t> const &raw) const
{
    auto index = detail::normalize(raw, _shape);
    int64_t pos = detail::calcNDIndex(index, _strides, _offset, this->ndim());
    return _buffer->operator[](pos);
}

/************************ slicing ************************/

/// @brief Return new NDArrayRef object after slicing.
/// @param raw.size() <= shape.size(). accepts both integers and slices
/// @return NDArrayRef object after slicing
/// @note Memory allocation: never.
template <typename dtype>
NDArrayRef<dtype> NDArrayRef<dtype>::slice(std::span<const Slice> raw) const
{
    int64_t ndim = this->ndim();
    std::vector<std::variant<Slice, int64_t>> indicies(ndim);
    for(int64_t i = 0; i < raw.size(); ++i) {
        indicies[i] = raw[i];
    }
    return this->slice(indicies);
}

/// @brief Return new NDArrayRef object after slicing.
/// @param raw.size() <= shape.size(). accepts both integers and slices
/// @return NDArrayRef object after slicing
/// @note Memory allocation: never.
template <typename dtype>
NDArrayRef<dtype> NDArrayRef<dtype>::slice(std::span<const int64_t> raw) const
{
    int64_t ndim = this->ndim();
    std::vector<std::variant<Slice, int64_t>> indicies(ndim);
    for(int64_t i = 0; i < raw.size(); ++i) {
        indicies[i] = raw[i];
    }
    return this->slice(indicies);
}

/// @brief Return new NDArrayRef object after slicing.
/// @param raw.size() <= shape.size(). accepts both integers and slices
/// @return NDArrayRef object after slicing
/// @note Memory allocation: never.
template <typename dtype>
NDArrayRef<dtype> NDArrayRef<dtype>::slice(std::vector<std::variant<Slice, int64_t>> const &raw) const
{
    int64_t old_ndim = this->ndim();
    if (raw.size() > old_ndim) {
        throw std::invalid_argument("invalid number of slices");
    }

    int64_t old_offset = _offset;
    auto const &old_strides = _strides;
    auto const &old_shape = _shape;

    int64_t new_offset;
    std::vector<int64_t> new_strides;
    std::vector<int64_t> new_shape;

    // indicies normalization
    auto indicies = detail::normalize(raw, old_shape);

    // calculate new offset
    std::vector<int64_t> begin_index(old_ndim);
    for (int64_t i = 0; i < old_ndim; ++i) {
        
        begin_index[i] = indicies[i].index() ? // indicies[i].index() determine the type of indicies[i]
            std::get<1>(indicies[i]) :
            std::get<0>(indicies[i]).start.value();
    }
    new_offset = detail::calcNDIndex(begin_index, old_strides, old_offset, old_ndim);

    // calculate new shape and new strides
    for (int64_t i = 0; i < old_ndim; ++i) {
        if (indicies[i].index() == 0) {
            auto &slice = std::get<0>(indicies[i]);
            new_strides.push_back(old_strides[i] * slice.step.value());
            new_shape.push_back(detail::calcNumel(slice, old_shape[i]));
        }
    }

    return NDArrayRef<dtype>(_buffer, std::move(new_shape), std::move(new_strides), new_offset);
}

/************************ permute ************************/

/// @brief Return a new array consisting of elements at specified indicies.
/// @param indicies.size() <= shape.size(). accepts both integers and slices
/// @return Array consisting of elements at specified indicies
/// @note Memory allocation: always, currently only one-dimensional arrays are supported.
/// @example Primitive array: 1, 2, 3, 4
///          Order of new dimensions: 3 0 2 1
///          New array: 4 1 3 2
template<typename dtype>
NDArrayRef<dtype> NDArrayRef<dtype>::permute(std::span<const int64_t> indicies) const
{
    if(this->ndim() <= 0) {
        throw std::invalid_argument("invalid ndim");
    }

    if(this->ndim() >  1) {
        // maybe implement this using concatenate
        throw std::invalid_argument("not implemented yet");
    }

    int64_t new_numel  = indicies.size();
    int64_t new_offset = 0;

    // currently only one-dimensional arrays are supported
    auto new_shape   = std::vector<int64_t>{ new_numel };
    auto new_buffer  = std::make_shared<BufferType>(new_numel);
    auto new_strides = detail::makeCompactStrides( new_shape );
    auto new_data    = new_buffer->data();

    for(int64_t i = 0; i < new_numel; ++i) {
        new_data[i] = this->elem( {indicies[i]} );
    }

    return { std::move(new_buffer), std::move(new_shape), std::move(new_strides), new_offset };
}

/// @brief Return a new array, where the elements are substituted by value at specified indices.
/// @param indicies.size() <= shape.size(). accepts both integers and slices
/// @param value Input value to substitute
/// @return Array, where the elements are substituted by value at specified indices
/// @note Memory allocation: always, currently only one-dimensional arrays are supported.
/// @example value: 1, 2, 3, 4
///          Order of new dimensions: 3 0 2 1
///          New array: 4 1 3 2
template<typename dtype>
NDArrayRef<dtype> NDArrayRef<dtype>::substitute(std::span<const int64_t> indicies, NDArrayRef<dtype> value) const
{
    if(this->ndim() <= 0) {
        throw std::invalid_argument("invalid ndim");
    }

    if(this->ndim() >  1) {
        // maybe implement this using concatenate
        throw std::invalid_argument("not implemented yet");
    }

    if(value.ndim() != 1) {
        throw std::invalid_argument("invalid value");
    }

    if(indicies.size() != value.numel()) {
        throw std::invalid_argument("invalid value");
    }

    auto new_arr = this->copy();
    for(int64_t i = 0; i < indicies.size(); ++i) {
        new_arr.elem( {indicies[i]} ) = value.elem( {i} );
    }
    return new_arr;
}

/************************ copy ************************/

/// @brief Return a compact array with the same content.
/// @return Compact array with the same content
/// @note Memory allocation: when this is not a compact array.
template <typename dtype>
NDArrayRef<dtype> NDArrayRef<dtype>::compact() const
{
    if( detail::isCompactStrides( this->strides(), this->shape() ) && this->offset() == 0 )
        return *this;
    else
        return this->copy();
}

/// @brief Return a compact copy of original data.
/// @return Compact copy of original data
/// @note Memory allocation: always.
template <typename dtype>
NDArrayRef<dtype> NDArrayRef<dtype>::copy() const
{
    auto const& old_buffer = _buffer;
    auto const& old_shape = _shape;
    auto const& old_offset = _offset;
    auto const& old_strides = _strides;

    auto new_data = std::make_shared<BufferType>();
    auto new_shape = old_shape;
    auto new_offset = 0;
    auto new_strides = detail::makeCompactStrides( new_shape );

    // for compact strides, use memcpy to avoid computing indicies
    if( detail::isCompactStrides( old_strides, old_shape) && std::is_trivially_copyable_v<dtype> ) {
        new_data->resize( numel() );

        auto dest = new_data->data();
        auto src = old_buffer->data() + old_offset;
        auto nbytes = sizeof(dtype) * numel();

        memcpy(dest, src, nbytes);
    }
    // for linear strides, use linear index to reduce indicies computation
    else if( detail::isLinearStrides( old_strides, old_shape ) ) {
        new_data->reserve( numel() );
        for(auto iter = this->lbegin(); iter != this->lend(); ++iter) {
            new_data->push_back( *iter );
        }
    }
    // for non-linear strides, copy one by one
    else {
        new_data->reserve( numel() );
        for(auto iter = this->begin(); iter != this->end(); ++iter) {
            new_data->push_back( *iter );
        }
    }

    return { std::move(new_data), std::move(new_shape), std::move(new_strides), new_offset };
}

/************************ reshape ************************/

/// @brief Readjust the NDArrayRef shape to the given new shape.
/// @param new_shape New shape of value
/// @return Array with given shape
/// @note Memory allocation: when this array does not have linear strides.
template <typename dtype>
NDArrayRef<dtype> NDArrayRef<dtype>::reshape(std::vector<int64_t> new_shape) const
{
    auto old_numel = this->numel();
    auto new_numel = detail::calcNumel(new_shape);
    if (new_numel != old_numel) {
        throw std::invalid_argument("invalid reshape");
    }

    auto const& old_buffer = this->_buffer;
    auto const& old_shape = this->_shape;
    auto const& old_offset = this->_offset;
    auto const& old_strides = this->_strides;

    // for linear strides, reshape can be done in place
    if( detail::isLinearStrides( old_strides, old_shape ) ) {
        auto new_buffer = old_buffer;
        auto new_offset = old_offset;
        auto new_strides = detail::makeLinearStrides(old_strides.back(), new_shape);
        return { std::move(new_buffer), std::move(new_shape), std::move(new_strides), new_offset };
    }
    // for non linear strides, copy is necassary
    else {
        return this->copy().reshape(std::move(new_shape));
    }

}

/// @brief Readjust the NDArrayRef shape to the given new shape.
/// @param new_shape New shape of value
/// @return Array with given shape
/// @note Memory allocation: never.
template <typename dtype>
NDArrayRef<dtype> NDArrayRef<dtype>::broadcast_to(std::vector<int64_t> new_shape) const {
    int64_t old_ndim = this->ndim();
    int64_t new_ndim = new_shape.size();

    if( old_ndim > new_ndim )
        throw std::invalid_argument("invalid broadcast");

    auto const& old_buffer = this->_buffer;
    auto const& old_shape = this->_shape;
    auto const& old_offset = this->_offset;
    auto const& old_strides = this->_strides;

    auto new_buffer = old_buffer;
    int64_t new_offset = old_offset;
    std::vector<int64_t> new_strides( new_ndim, 0 );

    for(int64_t i = 0; i < old_ndim; ++i) {
        if( new_shape[ new_ndim - 1 - i ] == old_shape[ old_ndim - 1 - i ] ) {
            new_strides[ new_ndim - 1 - i ] = old_strides[ old_ndim - 1 - i ];
        } else if ( old_shape[ old_ndim - 1 - i ] == 1 ) {
            //stride is set to 0, no memory allocation is required
            new_strides[ new_ndim - 1 - i ] = 0;
        } else {
            throw std::invalid_argument("invalid broadcast");
        }
    }

    return { std::move(new_buffer), std::move(new_shape), std::move(new_strides), new_offset };
}

/// @brief Return a new array after the order of the swapped dimensions.
/// @return New array after the order of the swapped dimensions
/// @note Memory allocation: never.
template <typename dtype>
NDArrayRef<dtype> NDArrayRef<dtype>::transpose() const {
    auto const& old_buffer  = this->_buffer;
    auto const& old_shape   = this->_shape;
    auto const& old_offset  = this->_offset;
    auto const& old_strides = this->_strides;

    auto new_buffer = old_buffer;
    std::vector<int64_t> new_shape { old_shape.rbegin(), old_shape.rend() };
    std::vector<int64_t> new_strides { old_strides.rbegin(), old_strides.rend() };
    int64_t new_offset = old_offset;

    return { std::move(new_buffer), std::move(new_shape), std::move(new_strides), new_offset };
}

/************************ type cast ************************/

/// @brief Return a new array with elements of new_typeM
/// @return New array with elements of new_typeM
/// @note Memory allocation: alwaysM
template <typename old_type>
template <typename new_type>
requires (std::constructible_from<new_type, old_type>)
NDArrayRef<new_type> NDArrayRef<old_type>::as() const
{
    int64_t numel = this->numel();
    auto const& old_strides = this->strides();
    auto const& old_shape   = this->shape();

    int64_t new_offset  = 0;
    auto    new_shape   = old_shape;
    auto    new_strides = detail::makeCompactStrides(new_shape);
    auto    new_buffer  = std::make_shared<typename NDArrayRef<new_type>::BufferType >(numel);
    auto    new_data    = new_buffer->data();

    if( detail::isLinearStrides( old_strides, old_shape ) )
    {
        // linear iterator, faster, only valid for array with linear strides
        auto iter = this->lbegin();
        for(int64_t i = 0; i < numel; ++i, ++iter) {
            new_data[i] = new_type( *iter );
        }
    }
    else
    {
        // general iterator, always valid
        auto iter = this->begin();
        for(int64_t i = 0; i < numel; ++i, ++iter) {
            new_data[i] = new_type( *iter );
        }
    }

    return { std::move(new_buffer), std::move(new_shape), std::move(new_strides), new_offset };
}

/************************ factory ************************/

/// @brief Generate a NDArrayRef object that contains a sequence of values within the specified range. 
/// @param start Start value of the sequence
/// @param stop End value of the sequence (not included in the sequence)
/// @param step Length between adjacent elements in the step sequence (default 1)
/// @return Contains the NDArrayRef object for a sequence of values within the specified range
template <typename dtype>
NDArrayRef<dtype> arange(dtype start, dtype stop, dtype step)
{
    assert(step != 0);

    // TODO fixed point, floating point
    // note: floating point does not support modular operation
    if (std::is_integral_v<dtype> == false) {
        throw std::runtime_error("arange for non int64_t type is not implemented yet");
    }

    // calculate number of elements
    int64_t numel;
    if (step > 0) {
        if (start >= stop)
            throw std::invalid_argument("emtpy range");
        auto q = (stop - start) / step;
        auto r = (stop - start) % step;
        numel = q + (r > 0);
    } else {
        if (stop <= start)
            throw std::invalid_argument("emtpy range");
        auto q = (start - stop) / (-step);
        auto r = (start - stop) % (-step);
        numel = q + (r > 0);
    }

    // calculate shape, strides, offset
    std::vector<int64_t> shape{numel};
    auto strides = detail::makeCompactStrides(shape);
    int64_t offset{0};

    // prepare data
    using namespace std::ranges::views;
    auto data = std::make_shared<typename NDArrayRef<dtype>::BufferType>();
    data->reserve(numel);
    for (auto x : iota(0, numel) | transform([start, step](auto i) -> dtype { return start + static_cast<dtype>(i) * step; })) {
        data->push_back(x);
    }

    return { std::move(data), std::move(shape), std::move(strides), offset };
}

/// @brief Make a nDArrayRef filled with 0.
/// @param shape Store new array shape information
/// @param expand Indicate whether to expand the NDArrayRef by repeating the value(default: false)
/// @return New ArrayRef
template <typename dtype>
NDArrayRef<dtype> zeros(std::vector<int64_t> shape, bool expand)
{
    return make_ndarray( static_cast<dtype>(0), std::move(shape), expand );
}

/// @brief Make a nDArrayRef filled with 1.
/// @param shape Store new array shape information
/// @param expand Indicate whether to expand the NDArrayRef by repeating the value(default: false)
/// @return New ArrayRef
template <typename dtype>
NDArrayRef<dtype> ones(std::vector<int64_t> shape, bool expand)
{
    return make_ndarray( static_cast<dtype>(1), std::move(shape), expand );
}

/// @brief Make a nDArrayRef filled with value.
/// @param shape Store new array shape information
/// @param value Used to initialize the elements of the NDArrayRef
/// @param expand Indicate whether to expand the NDArrayRef by repeating the value(default: false)
/// @return New ArrayRef
template <typename dtype>
NDArrayRef<dtype> make_ndarray (dtype const& value, std::vector<int64_t> shape, bool expand)
{
    if(expand)
    {
        int64_t offset  = 0;
        int64_t numel   = detail::calcNumel(shape);
        auto    buffer  = std::make_shared<typename NDArrayRef<dtype>::BufferType>(numel, value);
        auto    strides = detail::makeCompactStrides(shape);
        return {std::move(buffer), std::move(shape), std::move(strides), offset };
    }
    else
    {
        auto buffer  = std::make_shared<typename NDArrayRef<dtype>::BufferType>(1, value);
        auto strides = detail::makeLinearStrides(0, shape);
        int64_t offset = 0;
        return { std::move(buffer), std::move(shape), std::move(strides), offset };
    }
}

/// @brief Make an empty NDArrayRef.
/// @param shape Store new array shape information
/// @return New NDArrayRef
template <typename dtype>
NDArrayRef<dtype> make_ndarray (std::vector<int64_t> shape)
{
    int64_t offset  = 0;
    auto    numel   = detail::calcNumel(shape);
    auto    strides = detail::makeCompactStrides(shape);
    auto    buffer  = std::make_shared< typename NDArrayRef<dtype>::BufferType >( numel );

    return { std::move(buffer), std::move(shape), std::move(strides), offset };
}

/// @brief Create a NDArrayRef object from a std::vector<dtype>.
/// @param data Containing elements to be assigned to the NDArrayRef
/// @return New NDArrayRef
template <typename dtype>
NDArrayRef<dtype> make_ndarray (std::vector<dtype> data)
{
    int64_t offset  = 0;
    auto    strides = std::vector<int64_t>{ 1 };
    auto    shape   = std::vector<int64_t>{ static_cast<int64_t>(data.size()) };
    auto    buffer  = std::make_shared< typename NDArrayRef<dtype>::BufferType >( std::move(data) );

    return { std::move(buffer), std::move(shape), std::move(strides), offset };
}

/// @brief Create a NDArrayRef object from a std::initializer_list<dtype>
/// @param data Containing elements to be assigned to the NDArrayRef
/// @return New NDArrayRef
template <typename dtype>
NDArrayRef<dtype> make_ndarray (std::initializer_list<dtype> data)
{
    int64_t offset  = 0;
    auto    strides = std::vector<int64_t>{ 1 };
    auto    shape   = std::vector<int64_t>{ static_cast<int64_t>(data.size()) };
    auto    buffer  = std::make_shared< typename NDArrayRef<dtype>::BufferType >( data );

    return { std::move(buffer), std::move(shape), std::move(strides), offset };
}

/************************ output ************************/

/// @brief Return a string representation of the ArrayRef .
/// @example ArrayRef<int> arr : {1, 2; 3, 4}, 
///          arr.to_string : "1 2
///                           3 4".
template <typename dtype>
std::string NDArrayRef<dtype>::to_string() const
{
    using namespace std::ranges::views;

    auto to_str = [](auto const& x) -> std::string {
        if constexpr ( requires { x.to_string(); } ) {
            return x.to_string();
        }
        else if constexpr ( requires { std::to_string(x); } ) {
            return std::to_string(x);
        }
        else {
            std::string name = typeid(x).name();
            std::string err = "unable to convert " + name + " to string";
            throw std::runtime_error(err);
        }
    };

    if (ndim() == 0) {
        return to_str( *this->begin() );
    }
    else if (ndim() == 1) {
        auto res = iota(0, numel()) | transform([this](int64_t i) { return this->elem({i}); }) | transform([&to_str](auto const &x) { return to_str(x); });
        std::string str = "";
        for(auto x : res) { str += x; str += " "; }
        return str;
    }
    else {
        // Converts array elements to strings and concatenates them into a string separated by Spaces.
        auto res = iota(0, shape(0)) | transform([this](int64_t i){ return this->slice({i}).to_string(); });
        std::string str = "";
        for(auto x : res) { str += x; str += "\n"; }
        return str;
    }
}

/************************ iterator ************************/

/// @brief General iterator to const element, always valid.
template <typename dtype>
NDIterator<const dtype> NDArrayRef<dtype>::begin() const
{
    return NDIterator<const dtype>(
        this->data(),
        this->offset(),
        0,
        std::vector<int64_t>( this->ndim(), 0 ),
        this->shape(),
        detail::makeLinearIncrement( this->strides(), this->shape() ));
}

/// @brief General iterator, always valid.
template <typename dtype>
NDIterator<dtype> NDArrayRef<dtype>::begin()
{
    return NDIterator<dtype>(
        this->data(),
        this->offset(),
        0,
        std::vector<int64_t>( this->ndim(), 0 ),
        this->shape(),
        detail::makeLinearIncrement( this->strides(), this->shape() ));
}

/// @brief General iterator to const element, always valid.
template <typename dtype>
NDIterator<const dtype> NDArrayRef<dtype>::end() const
{
    return NDIterator<const dtype>(
        this->data(), {},
        this->numel(), {}, {}, {} );
}

/// @brief General iterator, always valid.
template <typename dtype>
NDIterator<dtype> NDArrayRef<dtype>::end()
{
    return NDIterator<dtype>(
        this->data(), {},
        this->numel(), {}, {}, {} );
}

/// @brief Linear iterator to const element, faster, only valid for array with linear/compact strides.
template <typename dtype>
LNIterator<const dtype> NDArrayRef<dtype>::lbegin() const
{
    if( detail::isLinearStrides(this->strides(), this->shape()) == false )
        throw std::invalid_argument("NDArray with non-linear strides does not have linear iterator");

    return LNIterator<const dtype>(
        this->data(),
        this->offset(),
        0,
        this->strides().back());
}

/// @brief Linear iterator, faster, only valid for array with linear/compact strides.
template <typename dtype>
LNIterator<dtype> NDArrayRef<dtype>::lbegin()
{
    if( detail::isLinearStrides(this->strides(), this->shape()) == false )
        throw std::invalid_argument("NDArray with non-linear strides does not have linear iterator");

    return LNIterator<dtype>(
        this->data(),
        this->offset(),
        0,
        this->strides().back());
}

/// @brief Linear iterator to const element, faster, only valid for array with linear/compact strides.
template <typename dtype>
LNIterator<const dtype> NDArrayRef<dtype>::lend() const
{
    return LNIterator<const dtype>(
        this->data(), {},
        this->numel(), {});
}

/// @brief Linear iterator, faster, only valid for array with linear/compact strides.
template <typename dtype>
LNIterator<dtype> NDArrayRef<dtype>::lend()
{
    return LNIterator<dtype>(
        this->data(), {},
        this->numel(), {});
}

} // namspace core
