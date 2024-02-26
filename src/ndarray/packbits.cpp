
#include <cmath>

#include "../tools/math.h"

#include "util.hpp"
#include "packbits.hpp"
#include "tools.hpp"

namespace core
{

///@brief performs packbits for the input NDArrayRef
///@param in NDArray to perform bit-packing operation.
///@param _axis Specifies the axis of the packaging operation, default no-value, which mean output is one-dimensional
///@return The packed NDArray, where each element represents the bit-packed result at the corresponding position in the input
NDArrayRef<uint8_t> packbits(NDArrayRef<uint8_t> const& in, std::optional<int64_t> _axis)
{
    int64_t     old_numel   = in.numel();
    int64_t     old_offset  = in.offset();
    auto        old_data    = in.data();
    auto const& old_shape   = in.shape();
    auto const& old_strides = in.strides();

    int64_t new_numel;
    int64_t new_offset;
    std::vector<int64_t> new_shape;
    std::vector<int64_t> new_strides;
    auto new_buffer = std::make_shared< NDArrayRef<uint8_t>::BufferType >();

    if( _axis.has_value() ) {
        int64_t ndim = in.ndim();
        int64_t axis = _axis.value();
        if( (-ndim <= axis && axis < ndim) == false ) {
            throw std::invalid_argument(fmt::format("invalid axis {}", axis));
        }
        if(axis < 0) axis += ndim;

        new_offset = 0;
        new_shape = old_shape;
        new_shape[axis] = ceildiv( old_shape[axis], 8 );
        new_strides = detail::makeCompactStrides(new_shape);
        new_numel = detail::calcNumel(new_shape);
        new_buffer->resize(new_numel);
        auto new_data = new_buffer->data();

        auto reduced_strides = new_strides;
        reduced_strides.erase( reduced_strides.begin() + axis );

        /* packing */
        for_each_axis(in, axis, [new_data, new_strides, new_offset, &reduced_strides, ndim, axis]
        (std::span<const int64_t> index, NDArrayRef<uint8_t> arr)
        {
            auto    sold_data   = arr.data();
            int64_t sold_numel  = arr.numel();
            int64_t sold_offset = arr.offset();
            auto    sold_stride = arr.strides().back();

            auto snew_offset = detail::calcNDIndex(index, reduced_strides, new_offset, ndim-1);
            auto snew_data   = new_data;
            auto snew_stride = new_strides[axis];

            for(int64_t i = 0; i < sold_numel/8; ++i) {
                snew_data[ snew_offset + i*snew_stride ] =
                    ( (sold_data[ sold_offset + (i*8+0) * sold_stride ] & 1) << 0) |
                    ( (sold_data[ sold_offset + (i*8+1) * sold_stride ] & 1) << 1) |
                    ( (sold_data[ sold_offset + (i*8+2) * sold_stride ] & 1) << 2) |
                    ( (sold_data[ sold_offset + (i*8+3) * sold_stride ] & 1) << 3) |
                    ( (sold_data[ sold_offset + (i*8+4) * sold_stride ] & 1) << 4) |
                    ( (sold_data[ sold_offset + (i*8+5) * sold_stride ] & 1) << 5) |
                    ( (sold_data[ sold_offset + (i*8+6) * sold_stride ] & 1) << 6) |
                    ( (sold_data[ sold_offset + (i*8+7) * sold_stride ] & 1) << 7);
            }

            if( sold_numel % 8 != 0 ) {
                uint8_t& x = snew_data[ snew_offset + sold_numel/8 * snew_stride ];
                x = 0;
                for(std::size_t i = sold_numel/8 * 8; i < sold_numel; ++i) {
                    x |= ( (sold_data[ sold_offset + i*sold_stride ] & 1) << (i%8) );
                }
            }
        });
    }
    else
    {
        new_numel   = ceildiv(old_numel, 8);
        new_offset  = 0;
        new_shape   = { new_numel };
        new_strides = { 1 };
        new_buffer->resize( new_numel );
        auto new_data = new_buffer->data();

        if( detail::isLinearStrides( old_strides, old_shape ) )
        {
            int64_t old_stride = old_strides.back();
            for(int64_t i = 0; i < old_numel/8; ++i) {
                new_data[i] =
                    ( (old_data[ old_offset + (i*8+0) * old_stride ] & 1) << 0) |
                    ( (old_data[ old_offset + (i*8+1) * old_stride ] & 1) << 1) |
                    ( (old_data[ old_offset + (i*8+2) * old_stride ] & 1) << 2) |
                    ( (old_data[ old_offset + (i*8+3) * old_stride ] & 1) << 3) |
                    ( (old_data[ old_offset + (i*8+4) * old_stride ] & 1) << 4) |
                    ( (old_data[ old_offset + (i*8+5) * old_stride ] & 1) << 5) |
                    ( (old_data[ old_offset + (i*8+6) * old_stride ] & 1) << 6) |
                    ( (old_data[ old_offset + (i*8+7) * old_stride ] & 1) << 7);
            }

            if( old_numel % 8 != 0 ) {
                uint8_t& x = new_data[ old_numel/8 ];
                x = 0;
                for(std::size_t i = old_numel/8 * 8; i < old_numel; ++i) {
                    x |= ( (old_data[ old_offset + i*old_stride ] & 1) << (i%8) );
                }
            }
        }
        else /* non-linear strides */
        {
            memset(new_data, 0, new_numel);
            for_each(in, [new_data](int64_t i, uint8_t x){
                new_data[ i/8 ] |= (x&1 << (i%8));
            });
        }
    }
    return { std::move(new_buffer), std::move(new_shape), std::move(new_strides), new_offset };
}

///@brief performs unpackbits for the input NDArrayRef
///@param in NDArray to perform bit-unpacking operation.
///@param _axis Specifies the axis of the packaging operation, default no-value, which mean output is one-dimensional
///@return Unpacked NDArray, where each element represents the bit unpacked result at the corresponding position in the input
NDArrayRef<uint8_t> unpackbits(NDArrayRef<uint8_t> const& in, std::optional<int64_t> _axis)
{
    int64_t old_numel = in.numel();
    int64_t old_offset = in.offset();
    auto old_data = in.data();
    auto const& old_shape = in.shape();
    auto const& old_strides = in.strides();

    int64_t new_numel;
    int64_t new_offset;
    std::vector<int64_t> new_shape;
    std::vector<int64_t> new_strides;
    auto new_buffer = std::make_shared< NDArrayRef<uint8_t>::BufferType >();

    if(_axis.has_value()) {
        int64_t ndim = in.ndim();
        int64_t axis = _axis.value();
        if( (-ndim <= axis && axis < ndim) == false ) {
            throw std::invalid_argument(fmt::format("invalid axis {}", axis));
        }
        if(axis < 0) axis += ndim;

        new_offset = 0;
        new_shape = old_shape;
        new_shape[axis] = old_shape[axis] * 8;
        new_strides = detail::makeCompactStrides(new_shape);
        new_numel = detail::calcNumel(new_shape);
        new_buffer->resize(new_numel);
        auto new_data = new_buffer->data();

        /* unpacking */

        // auto reduced_shape = old_shape;  reduced_shape[axis] = 1;
        // for(int64_t i = 0; i < detail::calcNumel(reduced_shape); ++i) {
        //     auto old_begin = detail::calcLinearIndex(i, reduced_shape, old_strides, old_offset);
        //     auto new_begin = detail::calcLinearIndex(i, reduced_shape, new_strides, new_offset);

        //     for(int64_t j = 0; j < old_shape[axis]; ++j) {
        //         new_data[ new_begin + (8*j+0)*new_strides[axis] ] = (old_data[ old_begin + j*old_strides[axis] ] >> 0) & 1;
        //         new_data[ new_begin + (8*j+1)*new_strides[axis] ] = (old_data[ old_begin + j*old_strides[axis] ] >> 1) & 1;
        //         new_data[ new_begin + (8*j+2)*new_strides[axis] ] = (old_data[ old_begin + j*old_strides[axis] ] >> 2) & 1;
        //         new_data[ new_begin + (8*j+3)*new_strides[axis] ] = (old_data[ old_begin + j*old_strides[axis] ] >> 3) & 1;
        //         new_data[ new_begin + (8*j+4)*new_strides[axis] ] = (old_data[ old_begin + j*old_strides[axis] ] >> 4) & 1;
        //         new_data[ new_begin + (8*j+5)*new_strides[axis] ] = (old_data[ old_begin + j*old_strides[axis] ] >> 5) & 1;
        //         new_data[ new_begin + (8*j+6)*new_strides[axis] ] = (old_data[ old_begin + j*old_strides[axis] ] >> 6) & 1;
        //         new_data[ new_begin + (8*j+7)*new_strides[axis] ] = (old_data[ old_begin + j*old_strides[axis] ] >> 7) & 1;
        //     }
        // }

        int64_t snew_stride = new_strides[axis];
        auto reduced_strides = new_strides;
        reduced_strides.erase( reduced_strides.begin() + axis );

        for_each_axis(in, axis, [new_data, new_offset, snew_stride, ndim, &reduced_strides]
        (std::span<const int64_t> index, NDArrayRef<uint8_t> arr)
        {
            int64_t snew_offset = detail::calcNDIndex(index, reduced_strides, new_offset, ndim-1);
            for_each(arr, [new_data, snew_offset, snew_stride](int64_t i, uint8_t x){
                new_data[ snew_offset + (8*i + 0) * snew_stride ] = (x >> 0) & 1;
                new_data[ snew_offset + (8*i + 1) * snew_stride ] = (x >> 1) & 1;
                new_data[ snew_offset + (8*i + 2) * snew_stride ] = (x >> 2) & 1;
                new_data[ snew_offset + (8*i + 3) * snew_stride ] = (x >> 3) & 1;
                new_data[ snew_offset + (8*i + 4) * snew_stride ] = (x >> 4) & 1;
                new_data[ snew_offset + (8*i + 5) * snew_stride ] = (x >> 5) & 1;
                new_data[ snew_offset + (8*i + 6) * snew_stride ] = (x >> 6) & 1;
                new_data[ snew_offset + (8*i + 7) * snew_stride ] = (x >> 7) & 1;
            });
        });
    }
    else
    {
        new_numel = old_numel * 8;
        new_offset = 0;
        new_shape = { new_numel };
        new_strides = { 1 };
        new_buffer->resize(new_numel);
        auto new_data = new_buffer->data();

        /* unpacking */
        for_each( in, [ new_data ](int64_t i, uint8_t x){
            new_data[8*i + 0] = (x >> 0) & 1;
            new_data[8*i + 1] = (x >> 1) & 1;
            new_data[8*i + 2] = (x >> 2) & 1;
            new_data[8*i + 3] = (x >> 3) & 1;
            new_data[8*i + 4] = (x >> 4) & 1;
            new_data[8*i + 5] = (x >> 5) & 1;
            new_data[8*i + 6] = (x >> 6) & 1;
            new_data[8*i + 7] = (x >> 7) & 1;
        });
    }

    return { std::move(new_buffer), std::move(new_shape), std::move(new_strides), new_offset };
}

} // namspace core
