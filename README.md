# PPPU
**Read this in other languages: [English](README.md), [中文](README.zh-CN.md).**

## Brief
An open-source privacy processing unit that currently provides basic computing and fixed-point number computing under the Semi2K protocol.

## Installation
### Preinstallation
* GNU Multi Precision 6.2.1 (built with C++ support)
* Boost 1.80
* OPENSSL (1.1.1 is suitable)
* github.com/fmtlib/fmt (9.1.0 is suitable)
* relic (for OT RCurve)
* google benchmark & googletest (for network test and context benchmark)

### Installation Steps
```
$ git clone https://github.com/qazw52/PPPU.git
$ unzip PPPU.zip
$ cd PPPU

Edit CMakeLists.txt and modify the paths for CMAKE_CXX_COMPILER(should be newest), boost, fmt, Google benchmark, and Google test in CMakeLists.txt to your own installation path.

$ mkdir build
$ cd build
$ cmake ..
$ make

For now the executable file is located in build.
```

### Quick Start
* Run tests
```
$ cd build
$ ./tutorial
```
## Layers
### Hierarchical Structure
```
                                Based on          Based on          Used in
Data Layer          : Datatype <-------- NDArray <-------- Context --------> Function Layer

                                     Based on          Used in
Communication Layer : Serialization <-------- Network --------> Function Layer

                          Based on
Function Layer      : OT <-------- PSI
                       |            |  Used in      Used in
                       -----------------------> ML <-------- Context, Network
                       |
                      MPC

                             Read Config                     Used in
Other Layer         : Config -----------> Network     Tools --------> All the other layers
                               Have examples in
                      Example <----------------- All the other layers
```

### Data Layer
#### Context
* Contains a **Value** object that encapsulates all the elements required for MPC computation, including the MPC protocol, plain data type, and shared data type.
* Each Value object can only be in three privacy states (**public, private, share**, the first two collectively referred to as **plain**), and each privacy state may have different Datatypes stored under different MPC protocols.
* **Basic** - Implemented arithmetic operations including input and output, bitwise decomposition, and addition, substraction, multiplication and so on, APIs are in **basic.h**.
  * The factory function APIs used to generate **Value** objects is located at **factory.h**.
  * The special implementation for arithmetic operations on fixed point numbers is located in **fxp.h**.
  * The base function and the wrap function based on privacy conditions and protocols is located at **raw.h**, **primitive.h** and **prot_wrapper.h** respectively.
  * Encode to Value and Decode to NDArray functions APIs are in **util.h**.
* **Compare** - Implemented comparation operations such as lesser and greater, APIs are in **compare.h**.
* **Math** - Implemented mathematical operations such as **division, exponential, logarithmic, polynomial, power, round to, sigmoid, sqrt** operations, APIs are in corresponding headers.
* **Shape** - Implemented shape changed operations such as **concatenate, reduce, sort**, APIs are in corresponding headers.

#### NDArray
* Contains a **NDArray** object that implements an n-dimensional array and also its computation, APIs are in **ndarray_ref.h**.
* Implementation of One-dimensional array is in **array_ref.h**. Data storage is in **buffer.h**.
* Other headers are the functions, such as **concatenate, iterator, packbits, serialization, slice**, APIs are in corresponding headers.

#### Datatype
* Contains **Z2k, Zp, FixedPoint** objects that implements a kinds of data types and their corresponding operations, APIs are in corresponding headers.
* The implementation of operations of the above-mentioned data types are stored in **mpx##.h**, based on gmp.
  * **Z2k** - A finite field whose modulus is 2^K, and provides basic operations and operations on finite fields, APIs are in **Z2k.h**.
  * **Zp** - A finite field whose modulus is prime, and provides basic operations and operations on finite fields, APIs are in **Zp.h**.
  * **FixedPoint** - Based on Z2k, a implementation of fixed point number, and provides basic operations, APIs are in **fix_point.h**.

### Communication Layer
#### Network
* Contains a **MultiPartyPlayer** object that implements communication among n party players. Also provides a **TwoPartyPlayer** object used between 2 party players.
* Both MultiPartyPlayer and TwoPartyPlayer have Secure and Plain version, which used SSL and TCP as its sockets respectively, which are contains real APIs used in computation, in **multi_party_player.h** and **two_party_player.h** respectively.
* Connection functions are in **mp_connect.h**, the sockets are in **socket_package.h**, player id are in **playerid.h** and communication information are in **statistics.h**. Other headers contains some implementations.

#### Serialization
* Contains a **Serializer** and a **Deserializer** used in communicaton. Serializer converts ArrayRef to ByteVector and Deserializer convert ByteVector to ArrayRef. ByteVector is used in communication. APIs are in **serializer.h** and **deserializer.h**.

### Function Layer
#### ML
* Machine learning functions based on PPPU. 

#### MPC
* Contains MPC protocols used in computations. APIs are in **protocol/protocol.hpp**. To use protocols, we have Protocol object and Preprocessing object to contains a protocol, APIs are in **protocol.hpp** and **preprocessing.hpp**.
* Now we implements **Semi2k** protocol.

#### PSI
* Contains **PSI** (Private Set Intersection) functions with now 2 algorithms (**crypto20, EcdhPSI**). APIs are in **algorithm/PSISender.h** and **algorithm/PSIReceiver.h**.

#### OT
* Contains base **OT** (Oblivious Transfer) functions. APIs are in **OTInterface.h** and **BasicDefines.h**.

### Other Layers
#### Tools
* Contains the math operations and data structures used in above layers.

#### Config
* Used to read network config based on boost program options.

## Protocol
### Semi2k-SPDZ
  A semi-honest MPC-protocol similar to SPDZ but requires a trusted third party to generate offline randoms. By default this protocol now uses trusted first party. Hence, it should be used for debugging purposes only.

## APIs
  For specific API instructions, please refer to the comments in the specific file. Here is only the function name, as for parameters and return values, please refer to the original file.

### Context

#### value.h
* **class Value<Protocol, pdtype, sdtype>**: The main unit of calculation in PPPU, based on NDArray.
* **fracbits() + set_fracbits()**: Getter & Setter of **fracbits** represents the decimal point position of a fixed point number, starting from the right.
* **visibility() + set_visibility()**: Getter & Setter of **vis** represents the visibility type of this Value.
* **is_public() + is_private() + is_plain() + is_share()**: Assert the visibility.
* **owner()**: Get the owner of the Value.
* **data_p/s() + assign_p/s()**: Getter & Setter of **data**.
* **ndim(), numel(), offset(), shape(), strides()**: Getter of **data**'s NDArray attributes.
* **copy(), slice(), reshape(), transpose(), broadcast_to(), permute(), substitute()**: Setter of **data**'s NDArray attributes, change them by different means.

#### visibility.h
* **class Visibility**: Indicates the public status of the Value. (Op: =)
* **type**: **VIS_PUBLIC** - Known to all parties, **VIS_PRIVATE** - Known to only one party, **VIS_SHARE** - Secret shared by all parties
* **is_public(), is_private(), is_share()**: Getter of **type**.
* **Public(), Private(), Share()**: Setter of **type**.
* **owner()**: Get the owner of the Visibility.

#### basic/basic.h
* **input()**: Input as a share value.
* **open()**: Open as a public value.
* **neg()**: Get the oppisite number of elements in the array of input Value.
* **add()**: Add two Value element by element.
* **sub()**: Subtract two Value element by element.
* **mul()**: Multiply two Value element by element.
* **square()**: Get the square of input Value element by element.
* **matmul()**: Matrix multiply two Value.
* **msb()**: Get the most significant bit.
* **eqz()**: Determine whether it is 0 element by element, set the corresponding bit to 1 if true.
* **sign()**: Get the sign bit.
* **abs()**: Get the absolute values of elements in the array of the input Value.
* **bitdec()**: Decompose decimal values in input Value into binary bits stored in a vector of Value.
* **h1bitdec()**: Decompose decimal values in input Value into binary bits stored in a vector of Value. Only record the highest bit in vector, others are set to 0.
* **bitcomp()**: Compose bit decomposed Value to a decimal Value.

#### basic/factory.h
* **make_public()**: Create a public Value object based on a NDArray or a Value (if it is float point type, set fracbits to -1).
* **make_private()**: Create a private Value object based on a NDArray or a Value or from other party (if it is float point type, set fracbits to -1, don't forget to set the owner).

#### basic/util.h
* **encode()**: Used to convert vector to built-in type.
* **decode()**: Used to convert built-in type to vector. (Used when printing the data.)
  
#### compare/compare.h
* **logical_not()**: Perform logical operation negation.
* **logical_and()**: Perform logical operation and.
* **logical_or()**: Perform logical operation or.
* **conditional()**: Used to get two mutually exclusive parts of two Value.
* **equal_to()**: Judge one's corresponding position is equal to anothor.
* **not_equal_to()**: Judge one's corresponding position is not equal to anothor.
* **less()**: Get the indicator vector of lesser value of corresponding position of the two inputs.
* **greater()**: Get the indicator vector of greater value of corresponding position of the two inputs.
* **less_equal()**: Get the indicator vector of lesser than or equal to value of corresponding position of the two inputs.
* **greater_equal()**: Get the indicator vector of greater than or equal to value of corresponding position of the two inputs.
* **min()**: Get the lesser value of corresponding position of the two inputs.
* **max()**: Get the greater value of corresponding position of the two inputs.

#### math/div.h
* **div()**: Do division operation a / b.
* **reciprocal()**: Get the reciprocal of input Value.

#### math/exp.h
* **exp()**: Get the x power of the base of natural logarithms.
* **exp2()**: Get the x power of 2.

#### math/log.h
* **log()**: Get the natural logarithm of input data.
* **log2()**: Get the base 2 logarithm of input data.
* **log10()**: Get the base 10 logarithm of input data.

#### math/polynomial.h
* **polynomial()**: Given x, get the value of a specific polynomial.

#### math/pow.h
* **pow()**: Get the y-th power of x Value.

#### math/round.h
* **fraction()**: Record the fracbits of input Value x.
* **floor()**: Round a number to an integer (round down).
* **ceil()**: Round a number to an integer (round up).
* **round()**: Round a number to an integer (half adjust).
* **mod()**: Perform modulo operation on Value x.

#### math/sigmoid.h
* **sigmoid()**: Get the sigmoid function of the input Value.

#### math/sqrt.h
* **sqrt()**: Get the square root of the input Value.

#### shape/concatenate.h
* **concatenate()**: Concatenate a set of Value end-to-end.

#### shape/reduce.h
* **sum()**: Sum all elements in the array of input Value.
* **min()**: Get the minimum value of all elements in the array of input Value. (Have different parameters with min() in compare.h.)
* **max()**: Get the maximum value of all elements in the array of input Value. (Have different parameters with max() in compare.h.)
* **argmax()**: Find the maximum value and its corresponding position in the array of input Value.

#### shape/sort.h
* **sort()**: Peforming odd-even merge sort on Value arr.

### NDArray

#### ndarray_ref.h
* **class NDArrayRef**: NDArrayRef class provides an interface for accessing and manipulating arrays, including data Pointers, number of elements, step size, offset, and so on. It also defines iterator types and associated operator overloads for easy traversal and access to elements in an array. In addition, it also provides some auxiliary methods, such as type conversion, slicing operations, etc.
* **ndim(), numel(), offset(), shape(), strides()**: Getter of this NDArray's attributes.
* **as(), compact(), copy(), slice(), reshape(), transpose(), broadcast_to(), permute(), substitute()**: Setter of this NDArray's attributes, change them by different means.
* **elem()**: Return a reference to the element at the specified index location.
* **sptr()**: Return a pointer to buffer.
* **data()**: Return a pointer to a contiguous memory region of elements.
* **begin(), end(), lbegin(), lend()**: General (or linear) iterator (to normal or const element), always valid.
* **make_ndarray()**: Make a NDArrayRef empty, or filled with value, a std::vector<dtype> or a std::initializer_list<dtype>.
* **arange()**: Generate a NDArrayRef object that contains a sequence of values within the specified range.
* **zeros(), ones()**: Make an NDArrayRef filled with 0 or 1. 

### Datatype

#### Z2k.h
* **class Z2<K, Signed>**: The 'Z2' class represents a finite field whose modulus is 2^K, and provides basic operations and operations on finite fields. K is a number of bits to represent the modulus 2^K. Signed is used to determine whether Z2 is a signed number or an unsigned number. (Op: +-*/, ~&|^, <<>>, +=, -=, *=, &=, |=, ^=, <<=, >>=, ==, =)
* **class Zp<size_t N_BITS>**: Have lesser function than Z2k, in Zp.h.
* **Z2()**: Allow implicit conversion from builtin types for convenience, or explicit conversion from floating-point types, different number of bits or different sign, mpz_class type values or strings and bases(default decimal).
* **double(), to_string(), to_mpz()**: Converts a finite field element to a double type representation, a string representation(default decimal) or an mpz_class type representation.
* **assign()**: Assigns a value of integral type, floating-point type, type mpz_class, strings and bases(default decimal) or different number of bits or different sign to a finite field element.
* **msb()**: Get the value of the bit at the most significant position in the _data.
* **bit()**: Get the value of the bit at the specified position in the _data.
* **data()**: Return the pointer of underlying data.

### Network

#### multi_party_player.h
* **two_party_player.h** is similiar with this module.
* **class MultiPartyPlayer**: Player entity under multi party communication conditions. It is a base class with basic network interface.
* **class SecureMultiPartyPlayer**: MultiPartyPlayer that uses SSL Socket as its socket.
* **class PlainMultiPartyPlayer**: MultiPartyPlayer that uses TCP Socket as its socket.
* **run()**: Run all of the players' threads.
* **connect()**: Interconnect the endpoints together.
* **stop()**: Stop the running threads.
* **setup_ssl_context()**: Used only in SecureMultiPartyPlayer to set up SSL protocol's context.
* **id(), all(), all_but_me(), num_players()**: Return my player id, all players' mpid, all players' mpid but mine, Return the number of players.
* **sync()**: Clear my buffer and sync with all other players.
* **send()**: Send message to another player.
* **msend()**: Send different messages to other players separately.
* **recv**: Receive message from another player.
* **mrecv**: Receive different messages from other players separately.
* **broadcast()**: Broadcast message to all the other players.
* **mbroadcast()**: Broadcast the same message to a group of players.
* **exchange()**: Send message to, then receive from another player.
* **pass_around()**: Send a message to the next player, and receive from the previous player.
* **broadcast_recv()**: Broadcast message, then receive from all other players.
* **mbroadcast_recv()**: Broadcast message, then receive among a group of players.
* **is_running()**: Judge whether the threads are running.
* **get_statistics()**: Get network statistics.

#### playerid.h
* **playerid_t**: playerid_t = std::size_t.

### MPC

#### protocol.hpp
* **class Protocol**: A protocol base class. Use to contains protocols.

#### preprocessing.hpp
* **class Preprocessing**: A preprocessing base class. Use to contains preprocessings.

#### semi2k/semi2k.hpp
* **class Semi2k**: A semi-honest MPC-protocol similar to SPDZ but requires a trusted third party to generate offline randoms.
* **class Semi2kTripl**: Multiplication triplet used in Semi2k protocol. As for the trusted third party.
* **plain(public, private), share**: As for **p, s** in function names, means computation is between or on which visibility.
* **input_p()**: Remote input or local private input for plain input from pid.
* **open_s()**: Open share to each party for share input.
* **neg_p/s()**: Negation for p/s input.
* **add_pp/sp/ss()**: Addition between p/s/s addend and p/p/s addend.
* **mul_pp/sp/ss()**: Multiplication between p/s/s multiplier and p/p/s multiplier.
* **matmul_pp/sp/ss()**: Matrix multiplication between p/s/s matrix multiplier and p/p/s matrix multiplier.
* **msb_p/s()**: Get the most significant bit for p/s input.
* **lshift_p/s()**: The left bit shift for p/s input.
* **trunc_p/s()**: The truncation for p/s input.
* **eqz_p/s()**: The equal to zero for p/s input.
* **bitdec_p/s()**: The bit decomposition for p/s input.
* **h1bitdec_p/s()**: The highest bit decomposition for p/s input.
