# **Context**
* Contains a **Value** object that encapsulates all the elements required for MPC computation, including the MPC protocol, plain data type, and shared data type.
* Each Value object can only be in three privacy states (**public, private, share**, the first two collectively referred to as plain), and each privacy state may have different Datatypes stored under different MPC protocols.
  ***
  ***
  ### **./context.h**
  ***
  #### **struct pppu::Config**
  Used to set parameters for fixed-point number calculation.
  ***
  #### **class pppu::Context**
  #### **pppu::Context(\*config, \*prot, \*prep, \*netio)**
  Constructor of Config.
  ##### **Parameters**
  * config - Used fixed-point number calculation parameters
  * prot - Used encryption protocol
  * prep - Used preprocessing
  * netio - Used network communication settings
  ***
  #### **Context.prot()**
  Get the encryption protocol used in calculation.
  ##### **Returns**
  * Encryption protocol used in calculation
  ***
  #### **Context.netio()**
  Get the network communication settings.
  ##### **Returns**
  * Network communication settings
  ***
  #### **Context.config()**
  Get the fixed-point number calculation parameters.
  ##### **Returns**
  * Fixed-point number calculation parameters
  ***
  #### **Context.fxp_fracbits()**
  Get the number of max binary bits used for fraction part.
  ##### **Returns**
  * Number of max binary bits used for fraction part
  ***
  #### **Context.pid()**
  Get the participant ID of a specific process.
  ##### **Returns**
  * The participant ID of a specific process
  ***
  #### **Context.num_parties()**
  Get the number of participants in communication.
  ##### **Returns**
  * The number of participants in communication
  ***
  ***
  ### **./value.h**
  ***
  #### **class Value**
  The main unit of calculation in PPPU, based on ndarray.
  ##### **Parameters**
  * _Protocol - Used encryption protocol
  * pdtype - Domain of plain text
  * sdtype - Domain of share text
  ***
  #### **Value.fracbits()**
  Get the fracbits of a Value object.
  ##### **Returns**
  * The fracbits parameter of this Value object
  ***
  #### **Value.set_fracbits(fracbits, force)**
  Set the fracbits of a Value object.
  ##### **Parameters**
  * fracbits - The fracbits of a Value object which is an integer greater than or equal to 0
  * force - Set it to true to force a Value change its fracbits when it already have one 
  ##### **Returns**
  * This Value object with set fracbits
  ***
  #### **Value.visibility()**
  Get the visibility of a Value object.
  ##### **Returns**
  * The visibility parameter of this Value object
  ***
  #### **Value.set_visibility(vis, force)**
  Set the visibility of a Value object.
  ##### **Parameters**
  * vis - The visibility of a Value object which is chosen from VIS_PUBLIC, VIS_PRIVATE and VIS_SHARE
  * force - Set it to true to force a Value change its visibility when it already have one 
  ##### **Returns**
  * This Value object with set visibility
  ***
  #### **Value.is_public()**
  Determine the visibility of value as public.
  ##### **Returns**
  * If it is public, return true, otherwise return false
  ***
  #### **Value.is_private()**
  Determine the visibility of value as private.
  ##### **Returns**
  * If it is private, return true, otherwise return false
  ***
  #### **Value.owner()**
  Get the owner of this Value.
  ##### **Returns**
  * The owner's player id
  ***
  #### **Value.is_plain()**
  Determine the visibility of value as public or private.
  ##### **Returns**
  * If it is public or private, return true, otherwise return false
  ***
  #### **Value.is_share()**
  Determine the visibility of value as share.
  ##### **Returns**
  * If it is share, return true, otherwise return false
  ***
  #### **Value.assign_p(data)**
  Assign the data of a Value with plain input.
  ##### **Parameters**
  * data - Plain input
  ##### **Returns**
  * This Value itself
  ***
  #### **Value.assign_s(data)**
  Assign the data of a Value with share input.
  ##### **Parameters**
  * data - Share input
  ##### **Returns**
  * This Value itself
  ***
  #### **Value.data_p()**
  Get the plain data of a Value.
  ##### **Returns**
  * Plain data of this Value
  ***
  #### **Value.data_s()**
  Get the share data of a Value.
  ##### **Returns**
  * Share data of this Value
  ***
  #### **Value.ndim()**
  Get the dimensions of the array in a Value object.
  ##### **Returns**
  * The dimensions of the array in this Value object
  ***
  #### **Value.numel()**
  Get the number of elements of the array in a Value object.
  ##### **Returns**
  * The number of elements of the array in this Value object
  ***
  #### **Value.offset()**
  Get the offset of a Value object.
  ##### **Returns**
  * The offset of this Value object
  ***
  #### **Value.shape(dim)**
  Get the shape of a Value object.
  ##### **Parameters**
  * dim - The dimension you want to get the shape
  ##### **Returns**
  * The shape of the specific dimension this Value object
  ***
  #### **Value.shape()**
  Get the shape of a Value object.
  ##### **Returns**
  * The shape of this Value object
  ***
  #### **Value.strides()**
  Get the strides of a Value object.
  ##### **Returns**
  * The strides of this Value object
  ***
  #### **Value.copy()**
  Used to copy a Value object.
  ##### **Returns**
  * The copied Value object
  ***
  #### **Value.slice(indicies)**
  Used to get parts of a whole Value object.
  ##### **Parameters**
  * indicies - The Slice object
  ##### **Returns**
  * The sliced Value object
  ***
  #### **Value.reshape(new_shape)**
  Used to turn a Value from one shape into another.
  ##### **Parameters**
  * new_shape - The new shape of this Value object
  ##### **Returns**
  * The reshaped Value object
  ***
  #### **Value.transpose()**
  Used to do matrix transpose.
  ##### **Returns**
  * The transposed Value object
  ***
  #### **Value.broadcast_to**
  Used to broadcast the array in a Value object to a new shape.
  ##### **Parameters**
  * new_shape - The new shape of this Value object
  ##### **Returns**
  * The broadcasted Value object
  ***
  #### **Value.permute(indicies)**
  Used to do multidimensional matrix permutation.
  ##### **Parameters**
  * indicies - The shape
  ##### **Returns**
  * The permuted Value object
  ***
  #### **Value.substitute(indicies, value)**
  Used to replace some contents in the array of a Value object.
  ##### **Parameters**
  * indicies - The shape
  * value - The replacing Value object
  ##### **Returns**
  * The replaced Value object
  ***
  #### **Value.to_string()**
  Used to print the stored data of a Value object.
  ##### **Returns**
  * The stored data of this Value object
  ***
  ***
  ### **visibility.h**
  ***
  #### **class Visibility**
  Indicates the public status of the Value.
  ##### **enum**
  * VIS_PUBLIC - Known to all parties
  * VIS_PRIVATE - Known to only one party
  * VIS_SHARE - Secret shared by all parties
  * VIS_INVALID - Invalid
  ***
  #### **pppu::Public()**
  Constructor of a public Visibility object.
  ***
  #### **pppu::Share()**
  Constructor of a share Visibility object.
  ***
  #### **pppu::Private(owner)**
  Constructor of a private Visibility object owned by an owner.
  ##### **Parameters**
  * owner - The owner of private Value object
  ***
  #### **Visibility.is_public()**
  Determine the visibility of value as public.
  ##### **Returns**
  * If it is public, return true, otherwise return false
  ***
  #### **Visibility.is_private()**
  Determine the visibility of value as private.
  ##### **Returns**
  * If it is private, return true, otherwise return false
  ***
  #### **Visibility.is_share()**
  Determine the visibility of value as share.
  ##### **Returns**
  * If it is share, return true, otherwise return false
  ***
  #### **Visibility.is_invalid()**
  Determine the visibility of value as invalid.
  ##### **Returns**
  * If it is invalid, return true, otherwise return false
  ***
  #### **Visibility.owner()**
  Get the owner of this Value.
  ##### **Returns**
  * The owner's player id
  ***
  ***
## **Basic**
* Implemented arithmetic operations including input and output, bitwise decomposition, and addition, substraction, multiplication and so on, APIs are in **basic.h**.
  * The factory function APIs used to generate Value objects is located at **factory.h**.
  * The special implementation for arithmetic operations on fixed point numbers is located in fxp.h.
  * The base function and the warp function based on privacy conditions and protocols is located at raw.h, primitive.h and prot_wrapper respectively.
  * Encode to Value and Decode to NDArray functions APIs are in **util.h**.
  ***
  ***
  ### **./basic/basic.h**
  ***
  #### **pppu::input(\*ctx, in)**
  Input a private value as a share value.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The private input Value object
  ##### **Returns**
  * The share Value
  ***
  #### **pppu::open(\*ctx, in)**
  Open a share value as a public value.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The share input Value object
  ##### **Returns**
  * The public Value
  ***
  #### **pppu::neg(\*ctx, in)**
  Get the oppisite number of elements in the array of input Value.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The input Value object
  ##### **Returns**
  * The netitive Value, $-$ in
  ***
  #### **pppu::add(\*ctx, lhs, rhs)**
  Add two Value element by element.
  ##### **Parameters**
  * ctx - The calculation settings
  * lhs - First addend
  * rhs - Second addend
  ##### **Returns**
  * The sum, lhs $+$ rhs
  ***
  #### **pppu::sub(\*ctx, lhs, rhs)**
  Subtract two Value element by element.
  ##### **Parameters**
  * ctx - The calculation settings
  * lhs - Minuend
  * rhs - Subtrahend
  ##### **Returns**
  * The difference, lhs $-$ rhs
  ***
  #### **pppu::mul(\*ctx, lhs, rhs)**
  Multiply two Value element by element.
  ##### **Parameters**
  * ctx - The calculation settings
  * lhs - First multiplier
  * rhs - Second multiplier
  ##### **Returns**
  * The product, lhs $\times$ rhs
  ***
  #### **pppu::square(\*ctx, in)**
  Get the square of input Value element by element.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The input Value object
  ##### **Returns**
  * The product, in$^2$
  ***
  #### **pppu::matmul(\*ctx, lhs, rhs)**
  Matrix multiply two Value.
  ##### **Parameters**
  * ctx - The calculation settings
  * lhs - First matrix multiplier
  * rhs - Second matrix multiplier
  ##### **Returns**
  * The matrix product, lhs $\cdot$ rhs
  ***
  #### **pppu::msb(\*ctx, in)**
  Get the most significant bit.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The input Value object
  ##### **Returns**
  * The result, $x < 0 \to 1, x \ge 0 \to 0$
  ***
  #### **pppu::eqz(\*ctx, in)**
  Determine whether it is 0 element by element, set the corresponding bit to 1 if true.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The input Value object
  ##### **Returns**
  * The result, $x = 0 \to 1, x \ne 0 \to 0$
  ***
  #### **pppu::sign(\*ctx, in)**
  Get the sign bit.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The input Value object
  ##### **Returns**
  * The result, $x < 0 \to -1, x \ge 0 \to 1$
  ***
  #### **pppu::abs(\*ctx, in)**
  Get the absolute values of elements in the array of the input Value.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The input Value object
  ##### **Returns**
  * The result, $abs(x)$
  ***
  #### **pppu::bitdec(\*ctx, in, nbits)**
  Decompose decimal values in input Value into binary bits stored in a vector of Value.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The input decimal Value object
  * nbits - Maximum decomposed binary bits
  ##### **Returns**
  * Lower $n$ bits decomposition of $x$
  ***
  #### **pppu::h1bitdec(\*ctx, in, nbits)**
  Decompose decimal values in input Value into binary bits stored in a vector of Value. However, only record the highest bit in vector, others are set to 0.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The input decimal Value object
  * nbits - Maximum decomposed binary bits
  ##### **Returns**
  * Lower $n$ bits decomposition of $bitfloor(x)$
  ***
  #### **pppu::bitcomp(\*ctx, in, fracbits)**
  Compose bit decomposed Value to a decimal Value.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The input bit decomposed Value
  * fracbits - Fixed point number decimal point position
  ##### **Returns**
  * The composed Value
  ***
  ***
  ### **./basic/factory.h**
  ***
  #### **pppu::make_public(\*ctx, data, fracbits)**
  Create a public Value object based on a plain Value.
  ##### **Parameters**
  * ctx - The calculation settings
  * data - The input plain Value object
  * fracbits - Fixed point number decimal point position
  ##### **Returns**
  * The generated public Value object
  ***
  #### **pppu::make_private(\*ctx, data, fracbits)**
  Create a private Value object based on a plain Value.
  ##### **Parameters**
  * ctx - The calculation settings
  * data - The input plain Value object
  * fracbits - Fixed point number decimal point position
  ##### **Returns**
  * The generated private Value object
  ***
  #### **pppu::make_private(\*ctx, owner)**
  Create a private empty Value object for a specific player.
  ##### **Parameters**
  * ctx - The calculation settings
  * owner - The player who owns the private Value
  ##### **Returns**
  * The generated private Value object
  ***
  #### **pppu::make_constant(\*ctx, data, shape, expand)**
  Create a constant public Value object based on a integral value.
  ##### **Parameters**
  * ctx - The calculation settings
  * data - The data of this constant Value
  * shape - The shape of this constant Value
  * expand - Whether expand this constant Value to this shape
  ##### **Returns**
  * The generated constant Value object
  ***
  #### **pppu::make_constant(\*ctx, data, shape, fracbits, expand)**
  Create a constant public Value object based on a floating point value.
  ##### **Parameters**
  * ctx - The calculation settings
  * data - The data of this constant Value
  * shape - The shape of this constant Value
  * expand - Whether expand this constant Value to this shape
  ##### **Returns**
  * The generated constant Value object
  ***
  #### **pppu::make_public(\*ctx, data)**
  Create a public Value object based on an integral NDArray.
  ##### **Parameters**
  * ctx - The calculation settings
  * data - The input plain Value object
  ##### **Returns**
  * The generated public Value object
  ***
  #### **pppu::make_public(\*ctx, data, fracbits)**
  Create a public Value object based on a floating point NDArray.
  ##### **Parameters**
  * ctx - The calculation settings
  * data - The input plain Value object
  * fracbits - Fixed point number decimal point position
  ##### **Returns**
  * The generated public Value object
  ***
  #### **pppu::make_private(\*ctx, data)**
  Create a private Value object based on an integral NDArray.
  ##### **Parameters**
  * ctx - The calculation settings
  * data - The input plain Value object
  ##### **Returns**
  * The generated priavte Value object
  ***
  #### **pppu::make_private(\*ctx, data, fracbits)**
  Create a private Value object based on a floating point NDArray.
  ##### **Parameters**
  * ctx - The calculation settings
  * data - The input plain Value object
  * fracbits - Fixed point number decimal point position
  ##### **Returns**
  * The generated private Value object
  ***
  ***
  ### **./basic/util.h**
  ***
  #### **pppu::detail::encode(fn, input)**
  Turn builtin integral/floating point type values to integral/floating point type values which Value used.
  ##### **Parameters**
  * Fn - Function T->U
  * input - The input NDArrayRef object
  ##### **Returns**
  * The decoded NDArrayRef object
  ***
  #### **pppu::detail::decode(fn, input)**
  Turn integral/floating point type values which Value used to builtin integral/floating point type values.
  ##### **Parameters**
  * Fn - Function U->T
  * input - The input NDArrayRef object
  ##### **Returns**
  * The encoded NDArrayRef object
  ***
  ***
## **Compare**
* Implemented comparation operations such as lesser and greater, APIs are in **compare.h**.
  ***
  ***
  ### **./compare/compare.h**
  ***
  #### **pppu::logical_not(\*ctx, in)**
  Perform logical operation negation.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The input Value object
  ##### **Returns**
  * The negtive input Value object
  ***
  #### **pppu::logical_and(\*ctx, lhs, rhs)**
  Perform logical operation and.
  * ctx - The calculation settings
  * lhs - The first participant Value in the calculation
  * rhs - The second participant Value in the calculation
  ##### **Returns**
  * The and operation result of lhs and rhs
  ***
  #### **pppu::logical_or(\*ctx, lhs, rhs)**
  Perform logical operation or.
  ##### **Parameters**
  * ctx - The calculation settings
  * lhs - The first participant Value in the calculation
  * rhs - The second participant Value in the calculation
  ##### **Returns**
  * The or operation result of lhs and rhs
  ***
  #### **pppu::conditional(\*ctx, cond, v0, v1)**
  Used to get two mutually exclusive parts of two Value.
  ##### **Parameters**
  * ctx - The calculation settings
  * cond - The conditional Value in the calculation
  * v0 - The first participant Value in the calculation
  * v1 - The second participant Value in the calculation
  ##### **Returns**
  * The result, $cond = 0 ? v0 : v1$
  ***
  #### **pppu::less(\*ctx, lhs, rhs)**
  Get the indicator vector of greater value of corresponding position of the two inputs.
  ##### **Parameters**
  * ctx - The calculation settings
  * lhs - The first participant Value in the calculation
  * rhs - The second participant Value in the calculation
  ##### **Returns**
  * The less operation result of lhs and rhs. Judge one by one whether the value of the corresponding position of the first participant is less than the value of the corresponding position of the second participant. If it is less than, the result of the corresponding position will be set to 1, and vice versa, will be set to 0 (The shape needs to be the same). The following operations have the same rule.
  ***
  #### **pppu::greater(\*ctx, lhs, rhs)**
  ##### **Parameters**
  * ctx - The calculation settings
  * lhs - The first participant Value in the calculation
  * rhs - The second participant Value in the calculation
  ##### **Returns**
  * The greater operation result of lhs and rhs
  ***
  #### **pppu::less_equal(\*ctx, lhs, rhs)**
  Get the indicator vector of lesser than or equal to value of corresponding position of the two inputs.
  ##### **Parameters**
  * ctx - The calculation settings
  * lhs - The first participant Value in the calculation
  * rhs - The second participant Value in the calculation
  ##### **Returns**
  * The less_equal operation result of lhs and rhs
  ***
  #### **pppu::greater_equal(\*ctx, lhs, rhs)**
  Get the indicator vector of greater than or equal to value of corresponding position of the two inputs.
  ##### **Parameters**
  * ctx - The calculation settings
  * lhs - The first participant Value in the calculation
  * rhs - The second participant Value in the calculation
  ##### **Returns**
  * The greater_equal operation result of lhs and rhs
  ***
  #### **pppu::equal_to(\*ctx, lhs, rhs)**
  Judge one's corresponding position is equal to anothor.
  ##### **Parameters**
  * ctx - The calculation settings
  * lhs - The first participant Value in the calculation
  * rhs - The second participant Value in the calculation
  ##### **Returns**
  * The equal_to operation result of lhs and rhs
  ***
  #### **pppu::not_equal_to(\*ctx, lhs, rhs)**
  Judge one's corresponding position is not equal to anothor.
  ##### **Parameters**
  * ctx - The calculation settings
  * lhs - The first participant Value in the calculation
  * rhs - The second participant Value in the calculation
  ##### **Returns**
  * The not_equal_to operation result of lhs and rhs
  ***
  #### **pppu::min(\*ctx, lhs, rhs)**
  Get the lesser value of corresponding position of the two inputs.
  ##### **Parameters**
  * ctx - The calculation settings
  * lhs - The first participant Value in the calculation
  * rhs - The second participant Value in the calculation
  ##### **Returns**
  * The min operation result of lhs and rhs. Judge one by one whether the value of the corresponding position of the first participant is less than the value of the corresponding position of the second participant. If it is less than, the result of the corresponding position will be set to the value of the corresponding position of the first participant, and vice versa, will be set to the value of the corresponding position of the second participant (The shape needs to be the same). The following operations have the same rule.
  ***
  #### **pppu::max(\*ctx, lhs, rhs)**
  Get the greater value of corresponding position of the two inputs.
  ##### **Parameters**
  * ctx - The calculation settings
  * lhs - The first participant Value in the calculation
  * rhs - The second participant Value in the calculation
  ##### **Returns**
  * The max operation result of lhs and rhs
  ***
  ***
## **Math**
* Implemented mathematical operations such as **division, exponential, logarithmic, polynomial, power, round to, sigmoid, sqrt** operations, APIs are in corresponding headers.
  ***
  ***
  ### **./math/div.h**
  ***
  #### **pppu::div(\*ctx, a, b)**
  Do division operation $a \div b$.
  ##### **Parameters**
  * ctx - The calculation settings
  * a - The input dividend Value object
  * b - The input divisor Value object, cannot be 0
  ##### **Returns**
  * The result $\frac a b$
  ***
  #### **reciprocal(\*ctx, in)**
  Get the reciprocal of input Value.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The input Value object, cannot be 0
  ##### **Returns**
  * The reciprocal of input Value
  ***
  ***
  ### **./math/exp.h**
  ***
  #### **pppu::exp(\*ctx, x)**
  Get the x power of the base of natural logarithms.
  ##### **Parameters**
  * ctx - The calculation settings
  * x - The power number
  ##### **Returns**
  * The result, $exp(x)$
  ***
  #### **pppu::exp2(\*ctx, x)**
  Get the x power of 2.
  ##### **Parameters**
  * ctx - The calculation settings
  * x - The power number
  ##### **Returns**
  * The result, $exp_2(x)$
  ***
  ***
  ### **./math/log.h**
  ***
  #### **pppu::log(\*ctx, x)**
  Get the natural logarithm of input data.
  ##### **Parameters**
  * ctx - The calculation settings
  * x - The input Value to be logarithmized
  ##### **Returns**
  * The result, $log(x)$
  ***
  #### **pppu::log2(\*ctx, x)**
  Get the base 2 logarithm of input data.
  ##### **Parameters**
  * ctx - The calculation settings
  * x - The input Value to be logarithmized
  ##### **Returns**
  * The result, $log_2(x)$
  ***
  #### **pppu::log10(\*ctx, x)**
  Get the base 10 logarithm of input data.
  ##### **Parameters**
  * ctx - The calculation settings
  * x - The input Value to be logarithmized
  ##### **Returns**
  * The result, $log_{10}(x)$
  ***
  ***
  ### **./math/polynomial.h**
  ***
  #### **pppu::polynomial(\*ctx, coef, x)**
  Given $x$, get the value of a specific polynomial.
  ##### **Parameters**
  * ctx - The calculation settings
  * coef - The polynomial coefficients
  * x - The unknown number
  ##### **Returns**
  * The result, $coef[0] + coef[1] \cdot x + coef[2] \cdot x^2 + \cdots$
  ***
  ***
  ### **./math/pow.h**
  ***
  #### **pppu::pow(\*ctx, x, y)**
  Get the y-th power of x Value.
  ##### **Parameters**
  * ctx - The calculation settings
  * x - The input Value object to be powered
  * y - The power number
  ##### **Returns**
  * The result, $x ^ y$
  ***
  ***
  ### **./math/round.h**
  ***
  #### **pppu::fraction(\*ctx, x)**
  Record the fracbits of input Value x.
  ##### **Parameters**
  * ctx - The calculation settings
  * x - The input Value object to be rounded
  ##### **Returns**
  * The result, $x.fracbits()$
  ***
  #### **pppu::floor(\*ctx, x, keep_fracbits)**
  Round a number to an integer (round down).
  ##### **Parameters**
  * ctx - The calculation settings
  * x - The input Value object to be rounded
  * keep_fracbits - Whether to preserve fracbits after converting to integers
  ##### **Returns**
  * The result, $floor(x)$
  ***
  #### **pppu::ceil(\*ctx, x, keep_fracbits)**
  Round a number to an integer (round up).
  ##### **Parameters**
  * ctx - The calculation settings
  * x - The input Value object to be rounded
  * keep_fracbits - Whether to preserve fracbits after converting to integers
  ##### **Returns**
  * The result, $ceil(x)$
  ***
  #### **pppu::round(\*ctx, x, keep_fracbits)**
  Round a number to an integer (half adjust).
  ##### **Parameters**
  * ctx - The calculation settings
  * x - The input Value object to be rounded
  * keep_fracbits - Whether to preserve fracbits after converting to integers
  ##### **Returns**
  * The result, $round(x)$
  ***
  #### **pppu::mod(\*ctx, x, modulus)**
  Perform modulo operation on Value x.
  ##### **Parameters**
  * ctx - The calculation settings
  * x - The input Value object to be moded
  * modules - The modulus number, needs to be a integer or floating point number
  ##### **Returns**
  * The result, $x\ mod\ m$
  ***
  ***
  ### **./math/sigmoid.h**
  ***
  #### **pppu::sigmoid(\*ctx, x)**
  Get the sigmoid function of the input Value.
  ##### **Parameters**
  * ctx - The calculation settings
  * x - The input Value object to be calculated
  ##### **Returns**
  * The result, $sig(x)$
  ***
  ***
  ### **./math/sqrt.h**
  ***
  #### **pppu::sqrt(\*ctx, x)**
  Get the square root of the input Value.
  ##### **Parameters**
  * ctx - The calculation settings
  * x - The input Value object to be calculated
  ##### **Returns**
  * The result, $sqrt(x)$
  ***
  ***
  ### **./math/tools.cpp**
  ***
  #### **combination(n, m)**
  Get combinatorial number of n and m.
  ##### **Parameters**
  * n - From n different elements
  * m - Extract m elements
  ##### **Returns**
  * The result, $C(n, m)$
  ***
  #### **bernoulli(n)**
  Get bernoulli number of n.
  ##### **Parameters**
  * n - The input number to be calculated
  ##### **Returns**
  * The result, $B(n)$
  ***
  ***
## **Shape**
* Implemented shape changed operations such as **concatenate, reduce, sort**, APIs are in corresponding headers.
  ***
  ***
  ### **./shape/concatenate**
  ***
  #### **pppu::concatenate(\*ctx, values, axis)**
  Concatenate a set of Value end-to-end.
  ##### **Parameters**
  * ctx - The calculation settings
  * values - A set of Value to be concatenated
  * axis - Concatenating axis, for example, in matrix (dim = 2), 0 is concatenated on the row and 1 is concatenated on the column
  ##### **Returns**
  * The Value after concatenating
  ***
  ***
  ### **./shape/reduce.h**
  ***
  #### **pppu::sum(\*ctx, in, axis)**
  Sum all elements in the array of input Value.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The input Value
  * axis - Use when the input is a matrix and need to get the sum of specific axis
  ##### **Returns**
  * The sum of all elements in the array of input Value, or the sum of specific axis
  ***
  #### **pppu::min(\*ctx, in, axis)**
  Get the minimum value of all elements in the array of input Value.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The input Value
  * axis - Use when the input is a matrix and need to get the minimum value of specific axis
  ##### **Returns**
  * The minimum value of all elements in the array of input Value, or the minimum value of specific axis
  ***
  #### **pppu::max(\*ctx, in, axis)**
  Get the maximum value of all elements in the array of input Value.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The input Value
  * axis - Use when the input is a matrix and need to get the maximum value of specific axis
  ##### **Returns**
  * The maximum value of all elements in the array of input Value, or the maximum value of specific axis
  ***
  #### **pppu::argmax(\*ctx, in)**
  Find the maximum value and its corresponding position in the array of input Value.
  ##### **Parameters**
  * ctx - The calculation settings
  * in - The input Value
  ##### **Returns**
  * The maximum value (single value) and its corresponding position (same shape with in) in the array of input Value
  ***
  ***
  ### **./shape/sort.h**
  ***
  #### **pppu::sort(\*ctx, arr)**
  Peforming odd-even merge sort on Value arr.
  ##### **Parameters**
  * ctx - The calculation settings
  * arr - The input Value to be sorted
  ##### **Returns**
  * The Value after sorting
  ***
  ***