#pragma once

#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <cstdlib>
#include <math.h>
#include <thread>

#include "tools/byte_vector.h"

#include "tutorial_1_prepare.cpp"

namespace tutorial {

void tutorial_5_ndarray() {

/*
    NDArrayRef is similar to ndarray in the numpy library in that it can generate one-dimensional or multidimensional arrays,
    and try to use shallow copies to generate ndarray objects to speed up operations.
    NDArrayRef can be thought of as the c++ version of ndarray in the numpy library, implementing most of the functionality
    of ndarray in the numpy library to support convenient vector operations.
*/
    
/*
    NDArrayRef is mainly about the separation of data storage and its interpretation, or the separation of copy and view, 
    similar to ndarray in numpy, NDArrayRef stores shape, stride, offset, but does not store byte order,
    C-order or Fortran-order.
*/

//  core::make_ndarray needs to pass vector as a parameter, and the Initialization List can be passed.
    std::vector<double> vec = {1.1, 2.2, 3.3, 4.4};
    core::NDArrayRef<double> ndarray = core::make_ndarray(vec);
    core::NDArrayRef<double> ndarray2 = core::make_ndarray({1.1, 2.2, 3.3, 4.4});
    fmt::print("{}\n", ndarray.to_string());

/*
    core::make_ndarray needs to pass shape as a parameter, shape is of type vector<int64_t>,
    and the Initialization List can be passed.
*/
    std::vector<int64_t> shape = {2, 3};
    core::NDArrayRef<double> ndarray_shape = core::make_ndarray<double>(2, shape);
    core::NDArrayRef<double> ndarray_shape2 = core::make_ndarray<double>(2, {2, 3});
    fmt::print("{}\n", ndarray_shape.to_string());
    core::NDArrayRef<double> ones = core::ones<double>(shape);
    core::NDArrayRef<double> zeros = core::zeros<double>(shape);

//  core::arange currently supports only integers
    core::NDArrayRef<int> ndarray_arange = core::arange<int>(0, 16, 2);
    fmt::print("{}\n", ndarray_arange.to_string());


    auto a = core::arange<int>(0, 16);
    auto b = a.reshape({4, 4});
    fmt::print("{}\n", b.to_string());

//  permute gets a new array from the index array, and the type of the index array should be vector<int64_t>.
    std::vector<int64_t> permute = {3, 2, 1, 10};
    auto c = a.permute(permute);
    fmt::print("{}\n", c.to_string());

/*
    core::Slice is a data type that contains start, stop, and step, all of which have default values.
    {} indicates a full index in positive order. step support for negative numbers, 
    and start and stop also support negative numbers for indexes that start in the opposite direction.
*/
    auto s1 = b.slice({ core::Slice{.stop=3}, {} });  // b[:3:, ::]
    auto s2 = b.slice({ 1, core::Slice{.step=-1} });  // b[1, ::-1]
    fmt::print("{}\n", s1.to_string());
    fmt::print("{}\n", s2.to_string());

/*
    Iterator supports pos and index. pos gets the position relative to the data store and index 
    gets the position interpreted by strides and offset.
*/
    auto slice_b = b.slice({ core::Slice{.step = 2}, core::Slice{.step = 2}});
    fmt::print("{}\n", slice_b.to_string());
    auto iter = slice_b.begin();
    ++iter;
    assert(*iter == 2);
    assert(iter.pos() == 2);

//  The index is divided into multidimensional index and linear index.
    std::vector<int64_t> index = iter.index();
    assert(index[0] == 0);
    assert(index[1] == 1);
    assert(iter.linear_index() == 1);

}

}
