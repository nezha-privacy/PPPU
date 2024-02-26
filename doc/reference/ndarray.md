# **NDArray**
* Contains a **NDArray** object that implements an n-dimensional array and also its computation, APIs are in **ndarray_ref.h**.
* Implementation of One-dimensional array is in **array_ref.h**. Data storage is in buffer.h.
* Other headers are the functions, such as **concatenate, iterator, packbits, serialization, slice**, APIs are in corresponding headers.
  ***
  ***
  ### **./ndarray_ref.h**
  ***
  #### **class core::NDArrayRef**
  NDArrayRef class provides an interface for accessing and manipulating arrays, including data Pointers, number of elements, step size, offset, and so on. It also defines iterator types and associated operator overloads for easy traversal and access to elements in an array. In addition, it also provides some auxiliary methods, such as type conversion, slicing operations, etc.
  ##### **Parameters**
  * dtype - Type of the element in the dtype array.
  ***
  #### **NDArrayRef.ndim()**
  Return number of dimensions.
  ##### **Returns**
  * Number of dimensions
  ***
  #### **NDArrayRef.numel()**
  Return number of elements.
  ##### **Returns**
  * Number of elements
  ***
  #### **NDArrayRef.shape()**
  Return shape of NDArray.
   ##### **Returns**
  * Shape of NDArray
  ***
  #### **NDArrayRef.shape(dim)**
  Return shape of NDArray on given dimension.
  ##### **Parameters**
  * dim - Given dimension
  ##### **Returns**
  * Shape of NDArray
  ***
  #### **NDArrayRef.strides()**
  Return strides of NDArray.
  ##### **Returns**
  * Strides of NDArray
  ***
  #### **NDArrayRef.offset()**
  Return offset of NDArray.
  ##### **Returns**
  * Offset of NDArray
  ***
  #### **NDArrayRef.slice(indicies)**
  Return new NDArrayRef object after slicing. Memory allocation: never.
  ##### **Parameters**
  * indicies - indicies.size() <= shape.size(). accepts both integers and slices
  ##### **Returns**
  * The slice of input
  ***
  #### **NDArrayRef.permute(indicies)**
  Return a new array consisting of elements at specified indicies. Memory allocation: always, currently only one-dimensional arrays are supported.
  ##### **Returns**
  * The permute of input
  ***
  #### **NDArrayRef.elem(index)**
  Return a reference to the element at the specified index location.
  ##### **Parameters**
  * index - index.size() == shape.size()
  ##### **Returns**
  * The reference to the element at the specified index location
  ***
  #### **NDArrayRef.reshape(new_shape)**
  Readjust the NDArrayRef shape to the given new shape. Memory allocation: when this array does not have linear strides.
  ##### **Parameters**
  * new_shape - New shape of input
  ##### **Returns**
  * Array with given shape
  ***
  #### **NDArrayRef.broadcast_to(new_shape)**
  Return a new array after the order of the swapped dimensions.
  ##### **Returns**
  * The new array after the order of the swapped dimensions
  ***
  #### **NDArrayRef.transpose()**
  Return a new array after the order of the swapped dimensions.
  ##### **Returns**
  * The new array after the order of the swapped dimensions
  ***
  #### **NDArrayRef.compact()**
  Return a compact array with the same content.
  ##### **Returns**
  * The compact array with the same content
  ***
  #### **NDArrayRef.copy()**
  Return a compact copy of original data.
  ##### **Returns**
  * The compact copy of original data
  ***
  #### **NDArrayRef.as()**
  Return a new array with elements of new_typeM
  ##### **Returns**
  * The new array with elements of new_typeM
  ***
  #### **NDArrayRef.sptr()**
  Return a pointer to buffer.
  ##### **Returns**
  * The pointer to buffer
  ***
  #### **NDArrayRef.data()**
  Return a pointer to a contiguous memory region of elements.
  ##### **Returns**
  * The pointer to a contiguous memory region of elements
  ***
  #### **NDArrayRef.to_string()**
  Return a string representation of the ArrayRef.
  ##### **Returns**
  * The string representation of input
  ***
  #### **NDArrayRef.begin(), NDArrayRef.end()**
  General iterator, always valid.
  ***
  #### **NDArrayRef.lbegin(), NDArrayRef.lend()**
  Linear iterator, faster, only valid for array with linear/compact strides.
  ***
  #### **core::make_ndarray(shape)**
  Make an empty NDArrayRef.
  ##### **Parameters**
  * shape - Store new array shape information
  ##### **Returns**
  * New NDArrayRef
  ***
  #### **core::make_ndarray(value, shape, expand)**
  Make a NDArrayRef filled with value.
  ##### **Parameters**
  * shape - Store new array shape information
  * value - Used to initialize the elements of the NDArrayRef
  * expand - Indicate whether to expand the NDArrayRef by repeating the value(default: false)
  ##### **Returns**
  * New ArrayRef
  ***
  #### **core::make_ndarray(data)**
  Create a NDArrayRef object from a std::vector<dtype> or a std::initializer_list<dtype>.
  ##### **Parameters**
  * data - Containing elements to be assigned to the NDArrayRef
  ##### **Returns**
  * New ArrayRef
  ***
  #### **core::arange(start, stop, step)**
  Generate a NDArrayRef object that contains a sequence of values within the specified range.
  ##### **Parameters**
  * start - Start value of the sequence
  * stop - End value of the sequence (not included in the sequence)
  * step - Length between adjacent elements in the step sequence (default 1)
  ##### **Returns**
  * Contains the NDArrayRef object for a sequence of values within the specified range
  ***
  #### **core::zeros(shape, expand)**
  Make a NDArrayRef filled with 0.
  ##### **Parameters**
  * shape - Store new array shape information
  * expand - Indicate whether to expand the NDArrayRef by repeating the value(default: false)
  ##### **Returns**
  * New ArrayRef
  ***
  #### **core::ones(shape, expand)**
  Make a NDArrayRef filled with 1.
  ##### **Parameters**
  * shape - Store new array shape information
  * expand - Indicate whether to expand the NDArrayRef by repeating the value(default: false)
  ##### **Returns**
  * New ArrayRef
  ***
  ***
  ### **./array_ref.h**
  ***
  #### **class core::ArrayRef**
  ArrayRef class provides an interface for accessing and manipulating arrays, including data Pointers, number of elements, step size, offset, and so on. It also defines iterator types and associated operator overloads for easy traversal and access to elements in an array. In addition, it also provides some auxiliary methods, such as type conversion, slicing operations, etc.
  ***
  #### **ArrayRef.sptr()**
  Get the shared pointer _data.
  ##### **Returns**
  * The shared pointer _data
  ***
  #### **ArrayRef.data()**
  Get the pointer to the data.
  ##### **Returns**
  * The pointer to the data
  ***
  #### **ArrayRef.numel()**
  Return number of elements.
  ##### **Returns**
  * Number of elements
  ***
  #### **ArrayRef.stride()**
  Return stride of Array.
  ##### **Returns**
  * Stride of Array
  ***
  #### **ArrayRef.offset()**
  Return offset of Array.
  ##### **Returns**
  * Offset of Array
  ***
  #### **ArrayRef.begin(), ArrayRef.end()**
  General iterator, always valid.
  ***
  #### **ArrayRef.lbegin(), ArrayRef.lend()**
  Linear iterator, faster, only valid for array with linear/compact strides.
  ***
  #### **ArrayRef.to_string()**
  Return a string representation of the ArrayRef.
  ##### **Returns**
  * The string representation of input
  ***
  #### **ArrayRef.as()**
  Convert ArrayRef<old_type> to ArrayRef<new_type>.
  ##### **Returns**
  * ArrayRef with the modified type
  ***
  #### **ArrayRef.slice(indicies)**
  Return new ArrayRef object after slicing.
  ##### **Parameters**
  * indicies - indicies.size() <= shape.size(). accepts both integers and slices
  ##### **Returns**
  * The slice of input
  ***
  #### **core::make_array(numel)**
  Make an empty ArrayRef.
  ##### **Parameters**
  * numel - The number of elements
  ##### **Returns**
  * New ArrayRef
  ***
  #### **core::make_array(value, numel, expand)**
  Make an ArrayRef filled with value.
  ##### **Parameters**
  * numel - The number of elements
  * value - Used to initialize the elements of the ArrayRef
  * expand - Indicate whether to expand the ArrayRef by repeating the value(default: false)
  ##### **Returns**
  * New ArrayRef
  ***
  #### **core::make_array(vec)**
  Create an ArrayRef object from a std::vector<dtype>.
  ##### **Parameters**
  * vec - Containing elements to be assigned to the ArrayRef
  ##### **Returns**
  * New ArrayRef
  ***
  #### **core::make_array(list)**
  Create an ArrayRef object from a std::initializer_list<dtype>.
  ##### **Parameters**
  * list - Containing elements to be assigned to the ArrayRef
  ##### **Returns**
  * New ArrayRef
  ***
  ***
  ### **./concatenate.h**
  ***
  #### **core::concatenate(arrays, axis)**
  Concatenate multiple NDArrayRef arrays.
  ##### **Parameters**
  * arrays - std::span of the NDArrayRef array to be connected
  * axis - Axis of the connection(default 0)
  ##### **Returns**
  * The result NDArrayRef
  ***
  ***
  ### **./iterator.hpp**
  ***
  #### **class core::NDIterator**
  N-Dim array iterator.
  ***
  #### **NDIterator.pos()**
  Return position of current element in memory.
  ##### **Returns**
  * Position of current element in memory
  ***
  #### **NDIterator.linear_index()**
  Return linear index of current element.
  ##### **Returns**
  * Linear index of current element
  ***
  #### **NDIterator.index()**
  Return n-dimensional index of current element.
  ##### **Returns**
  * N-dimensional index of current element
  ***
  #### **class core::LNIterator**
  Return linear index of current element.
  ##### **Returns**
  * Linear index of current element
  ***
  ***
  ### **./packbits.hpp**
  ***
  #### **core::packbits(in, _axis)**
  Performs packbits for the input NDArrayRef.
  ##### **Parameters**
  * in - NDArray to perform bit-packing operation.
  * _axis - Specifies the axis of the packaging operation, default no-value, which mean output is one-dimensional
  ##### **Returns**
  * The packed NDArray, where each element represents the bit-packed result at the corresponding position in the input.
  ***
  #### **core::unpackbits(in, _axis)**
  Performs unpackbits for the input NDArrayRef
  ##### **Parameters**
  * in - NDArray to perform bit-unpacking operation.
  * _axis - Specifies the axis of the packaging operation, default no-value, which mean output is one-dimensional
  ##### **Returns**
  * The unpacked NDArray, where each element represents the bit-unpacked result at the corresponding position in the input.
  ***
  ***
  ### **./slice.hpp**
  ***
  #### **struct core::Slice**
  Represents a slice with optional start, stop, and step values. Allow negative start and stop (like python). Step default 1; if step > 0, start default 0, stop default n, if step < 0, start default n-1, stop default -1.
  ***
  ***