# **Datatypes**
* Contains **Z2k, Zp, FixedPoint** objects that implements a kinds of data types and their corresponding operations, APIs are in corresponding headers.
* The implementation of operations of the above-mentioned data types are stored in mpx##.h, based on gmp.
  * **Z2k** - A finite field whose modulus is 2^K, and provides basic operations and operations on finite fields, APIs are in **Z2k.h**.
  * **Zp** - A finite field whose modulus is prime, and provides basic operations and operations on finite fields, APIs are in **Zp.h**.
  * **FixedPoint** - Based on Z2k, a implementation of fixed point number, and provides basic operations, APIs are in **fix_point.h**.
  ***
  ***
  ### **./Z2k.h**
  ***
  #### **class Z2**
  The 'Z2' class represents a finite field whose modulus is 2^K, and provides basic operations and operations on finite fields.
  ##### **Parameters**
  * K - A number of bits to represent the modulus 2^K
  * Signed - Used to determine whether Z2 is a signed number or an unsigned number
  ***
  #### **Z2()**
  If K is multiple of 64,then this does not introduce any cost.
  ***
  #### **Z2(Int)**
  Allow implicit conversion from builtin types for convenience.
  ***
  #### **Z2(Float)**
  Explicit conversion from floating-point types.
  ***
  #### **Z2(other)**
  Explicit conversion for different number of bits or different sign.
  ***
  #### **Z2(mpz_class)**
  Constructs finite field elements using mpz_class type values.
  ***
  #### **Z2(std::string, base)**
  Constructs finite field elements using strings and bases(default decimal).
  ##### **Parameters**
  * base - The base of number
  ***
  #### **Z2.to_string(base)**
  Converts a finite field element to a string representation(default decimal).
  ##### **Parameters**
  * base - The base of number
  ##### **Returns**
  * The generated string
  ***
  #### **Z2.to_mpz()**
  Converts a finite field element to an mpz_class type representation.
  ##### **Returns**
  * The generated mpz_class
  ***
  #### **Z2.assign(Int)**
  Assigns a value of integral type to a finite field element.
  ***
  #### **Z2.assign(Float)**
  Assigns a value of floating-point type to a finite field element.
  ***
  #### **Z2.assign(mpz_class)**
  Assigns to finite field elements using mpz_class.
  ***
  #### **Z2.assign(std::string, base)**
  Assigns to finite field elements using strings and bases(default decimal).
  ##### **Parameters**
  * base - The base of number
  ***
  #### **Z2.assign(other)**
  Constructs finite field elements for different number of bits or different sign.
  ##### **Parameters**
  * other - Input with different size or sign.
  ***
  #### **Z2.msb()**
  Get the value of the bit at the most significant position in the _data.
  ##### **Returns**
  * The value of the bit at the most significant position
  ***
  #### **Z2.bit(index)**
  Get the value of the bit at the specified position in the _data. Support operation like { Z2<K, S> ans(0); ans.bit(K - 1) = 1; }
  ##### **Returns**
  * The value of the bit at the specified position
  ***
  #### **Z2.data()**
  Get the pointer of underlying data.
  ##### **Returns**
  * The pointer of underlying data
  ***
  #### **Z2.value_type()**
  Converts a finite field element to a value_type representation.
  ***
  #### **Z2.T()**
  Converts a finite field element to a double type representation.
  ***
  #### **inv(x)**
  Calculate the multiplicative inverse of a given unsigned binary integer.
  ##### **Parameters**
  * x - Input Z2 reference
  ##### **Returns**
  * The multiplicative inverse of a given unsigned binary integer
  ***
  #### **sqrt(x)**
  Calculate the square root of a given unsigned binary integer.
  ##### **Parameters**
  * x - Input Z2 reference
  ##### **Returns**
  * The square root of a given unsigned binary integer
  ***
  #### **abs(x)**
  Calculate the absolute value of a given binary integer.
  ##### **Parameters**
  * x - Input Z2 reference
  ##### **Returns**
  * The absolute value of a given binary integer
  ***
  #### **countl_zero(x)**
  Count the number of consecutive zeros starting from the significant bit-by-bit.
  ##### **Parameters**
  * x - Input Z2 reference
  ##### **Returns**
  * The number of consecutive zeros
  ***
  ***
  ### **./Zp.h**
  ***
  #### **class Zp**
  The 'Zp' class represents a finite field whose modulus is prime, and provides basic operations and operations on finite fields.
  ##### **Parameters**
  * N_BITS - is a number of bits to represent the modulus p.
  ***
  #### **Zp.init(modulus)**
  Initializes the modulus of a finite field. modulus is assigned to _modulus_class and _modulus[N_LIMBS].
  ##### **Parameters**
  * modulus - The mpz_class representation
  ***
  #### **Zp()**
  All computation's correctness counts on correct input, thus proper initialization is required.
  ***
  #### **Zp(unsigned long)**
  Constructs finite field elements using unsigned long integer values.
  ***
  #### **Zp(mpz_class)**
  Constructs finite field elements using mpz_class type values.
  ***
  #### **Zp(std::string, base)**
  Constructs finite field elements using strings and bases(default decimal).
  ##### **Parameters**
  * base - The base of number
  ***
  #### **to_string(10)**
  Converts a finite field element to a string representation(default decimal).
  ##### **Parameters**
  * base - The base of number
  ##### **Returns**
  * The string representation of input
  ***
  #### **Zp.to_mpz()**
  Converts a finite field element to an mpz_class type representation.
  ##### **Returns**
  * The mpz_class type representation of input
  ***
  #### **Zp.assign(unsigned long)**
  Assigns a value of type unsigned long to a finite field element.
  ***
  #### **Zp.assign(mpz_class)**
  Assigns a value of type mpz_class to a finite field element.
  ***
  #### **Zp.assign(std::string, base)**
  Assigns to finite field elements using strings and bases(default decimal).
  ##### **Parameters**
  * base - The base of number
  ***
  ***
  ### **./fixed_point.h**
  ***
  #### **class FixedPoint**
  A fixed point data type.
  ***
  #### **FixedPoint.underlying()**
  Return the underlying data.
  ##### **Returns**
  * The underlying data
  ***
  #### **FixedPoint(val)**
  Conversion from builtin float/double. Allow implicit conversion for convenience.
  ##### **Parameters**
  * val - Float/double input
  ***
  #### **FixedPoint.to_mpf()**
  Conversion to/from gmp mpf_class. Calculate the highest non-0 mp_bits_per_limb bits.
  ##### **Returns**
  * GMP mpf_class representation.
  ***
  #### **FixedPoint.to_string(base)**
  Conversion to/from std::string( default decimal ).
  ##### **Parameters**
  * base - The base of number
  ##### **Returns**
  * String representation
  ***
  ***