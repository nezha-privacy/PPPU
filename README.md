# PPPU
An open-source privacy processing unit that currently provides basic computing and fixed-point number computing under the semi-honest SPDZ2k protocol. The project is currently in the testing phase.  
**Read this in other languages: [English](README.md), [中文](README.zh-CN.md).**

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

Try install.sh to install preinstallation requirments. If you already install some of them, modify the path of it in CMakeLists.txt.

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


