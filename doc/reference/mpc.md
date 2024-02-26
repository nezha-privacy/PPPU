# **MPC**
* Contains MPC protocols used in computations. APIs are in **protocol/protocol.h**. To use protocols, we have Protocol object and Preprocessing object to contains a protocol, APIs are in **protocol.h** and **preprocessing.h**.
* Now we implements **Semi2k** protocol.
  ***
  ***
  ### **./protocol.hpp**
  ***
  #### **class mpc::Protocol**
  A preprocessing base class. Use to contains preprocessings.
  ***
  #### **struct mpc::isValidProtocolPlainType**
  Check whether ptype is a valid plain type of protocol and each specific protocol do its own specilization.
  ***
  #### **struct mpc::convertProtocolPlainTypeToShareType**
  Convert plain type to corresponding share type and each specific protocol do its own specilization with member type "type".
  ***
  #### **struct mpc::isValidProtocolPlainSharePair**
  Check whether ptype and stype is match and each specific protocol do its own specilization.
  ***
  ***
  ### **./preprocessing.hpp**
  ***
  #### **class mpc::Preprocessing**
  A preprocessing base class. Use to contains preprocessings.
  ***
  ***
  ### **./random_generator.h**
  ***
  #### **class RandomGenerator**
  A class used to generate random numbers.
  ***
  #### **RandomGenerator(seed)**
  The constructor.
  ##### **Parameters**
  * seed - The random seed
  ***
  #### **RandomGenerator.get_random()**
  Get a random number.
  ##### **Returns**
  * A random number
  ***
  ***
  ### **./semi2k/semi2k.hpp**
  ***
  #### **class mpc::Semi2kTriple**
  Multiplication triplet used in Semi2k protocol.
  ***
  #### **Semi2kTriple.get_n_triple(n)**
  Get n triples for Semi2k protocol.
  ##### **Parameters**
  * n - How many triples we need
  ##### **Returns**
  * n triples
  ***
  #### **Semi2kTriple.get_matrix_triple(M, N, KK)**
  Get matrix bever triple for Semi2k protocol.
  ##### **Parameters**
  * M - The first parameter
  * N - The second parameter
  * KK - The third parameter
  ##### **Returns**
  * Matrix bever triple
  ***
  #### **Semi2kTriple.get_n_randbit(n)**
  Try to get n randbits. Return true if successful, false otherwise
  ##### **Parameters**
  * n - How many randbits we need
  ##### **Returns**
  * n randbits
  ***
  #### **Semi2kTriple.get_r_and_rr(num)**
  Try to get n randbits. Return true if successful, false otherwise
  ##### **Parameters**
  * num - How many randbits we need
  ##### **Returns**
  * num pair of randbits
  ***
  #### **class mpc::Semi2k**
  A implementation of secure multi-party computation with protocol Semi2k.
  ***
  #### **Semi2k(\*netio, \*triples)**
  Constructor for overall settings.
  ##### **Parameters**
  * netio - Used network communication settings
  * triples - Multiplication triplet used in Semi2k protocol
  ***
  #### **Semi2k(playerid, n_players, \*mplayer, \*triples)**
  Constructor for single player.
  ##### **Parameters**
  * playerid - Player's id, from 0 to n_players - 1
  * n_players - Number of players.
  * mplayer - Implementation of features of multi player communication
  * triples - Multiplication triplet used in Semi2k protocol
  ***
  #### **Semi2k.input_p(pid, numel)**
  Implementation of remote input for plain input from pid under the Semi2k protocol.
  ##### **Parameters**
  * pid - Input from which player
  * numel - Number of input elements
  ##### **Returns**
  * ArrayRef object of input data
  ***
  #### **Semi2k.input_p(in)**
  Implementation of local private input for plain input under the Semi2k protocol.
  ##### **Parameters**
  * in - Local private input
  ##### **Returns**
  * ArrayRef object of input data
  ***
  #### **Semi2k.open_s(in)**
  Implementation of open share to each party for share input under the Semi2k protocol.
  ##### **Parameters**
  * in - Share input
  ##### **Returns**
  * ArrayRef object of input data
  ***
  #### **Semi2k.neg_p(in)**
  Implementation of negation for plain input under the Semi2k protocol.
  ##### **Parameters**
  * in - Plain input
  ##### **Returns**
  * ArrayRef object of the result, $-in$
  ***
  #### **Semi2k.neg_s(in)**
  Implementation of negation for share input under the Semi2k protocol.
  ##### **Parameters**
  * in - Share input
  ##### **Returns**
  * ArrayRef object of the result, $-in$
  ***
  #### **Semi2k.add_pp(lhs, rhs)**
  Implementation of addition between plain addend and plain addend under the Semi2k protocol.
  ##### **Parameters**
  * lhs - First plain addend
  * rhs - Second plain addend
  ##### **Returns**
  * ArrayRef object of the sum, $lhs + rhs$
  ***
  #### **Semi2k.add_sp(lhs, rhs)**
  Implementation of addition between share addend and plain addend under the Semi2k protocol.
  ##### **Parameters**
  * lhs - First share addend
  * rhs - Second plain addend
  ##### **Returns**
  * ArrayRef object of the sum, $lhs + rhs$
  ***
  #### **Semi2k.add_ss(lhs, rhs)**
  Implementation of addition between share addend and share addend under the Semi2k protocol.
  ##### **Parameters**
  * lhs - First share addend
  * rhs - Second share addend
  ##### **Returns**
  * ArrayRef object of the sum, $lhs + rhs$
  ***
  #### **Semi2k.mul_pp(lhs, rhs)**
  Implementation of multiplication between plain multiplier and plain multiplier under the Semi2k protocol.
  ##### **Parameters**
  * lhs First plain multiplier
  * rhs Second plain multiplier
  ##### **Returns**
  * ArrayRef object of the product, $lhs \times rhs$
  ***
  #### **Semi2k.mul_sp(lhs, rhs)**
  Implementation of multiplication between share multiplier and plain multiplier under the Semi2k protocol.
  ##### **Parameters**
  * lhs First share multiplier
  * rhs Second plain multiplier
  ##### **Returns**
  * ArrayRef object of the product, $lhs \times rhs$
  ***
  #### **Semi2k.mul_ss(lhs, rhs)**
  Implementation of multiplication between share multiplier and share multiplier under the Semi2k protocol.
  ##### **Parameters**
  * lhs First share multiplier
  * rhs Second share multiplier
  ##### **Returns**
  * ArrayRef object of the product, $lhs \times rhs$
  ***
  #### **Semi2k.msb_p(in)**
  Implementation of the most significant bit for plain input under the Semi2k protocol.
  ##### **Parameters**
  * in - Plain input
  ##### **Returns**
  * ArrayRef object of the result, $x < 0 \to 1, x \ge 0 \to 0$
  ***
  #### **Semi2k.msb_s(in)**
  Implementation of the most significant bit for share input under the Semi2k protocol.
  ##### **Parameters**
  * in - Share input
  ##### **Returns**
  * ArrayRef object of the result, $x < 0 \to 1, x \ge 0 \to 0$
  ***
  #### **Semi2k.lshift_p(in, nbits)**
  Implementation of the left bit shift for plain input under the Semi2k protocol.
  ##### **Parameters**
  * in - Plain input
  * nbits - Binary bits to shift
  ##### **Returns**
  * ArrayRef object of the result, $x << nbits$
  ***
  #### **Semi2k.lshift_s(in, nbits)**
  Implementation of the left bit shift for share input under the Semi2k protocol.
  ##### **Parameters**
  * in - Share input
  * nbits - Binary bits to shift
  ##### **Returns**
  * ArrayRef object of the result, $x << nbits$
  ***
  #### **Semi2k.trunc_p(in, nbits)**
  Implementation of the truncation for plain input under the Semi2k protocol.
  ##### **Parameters**
  * in - Plain input
  * nbits - Binary bits to shift
  ##### **Returns**
  * ArrayRef object of the result, $x >> nbits$
  ***
  #### **Semi2k.trunc_s(in, nbits)**
  Implementation of the truncation for share input under the Semi2k protocol.
  ##### **Parameters**
  * in - Share input
  * nbits - Binary bits to shift
  ##### **Returns**
  * ArrayRef object of the result, $x >> nbits$
  ***
  #### **Semi2k.eqz_p(in)**
  Implementation of the equal to zero for plain input under the Semi2k protocol.
  ##### **Parameters**
  * in - Plain input
  ##### **Returns**
  * ArrayRef object of the result, $x = 0 \to 1, x \ne 0 \to 0$
  ***
  #### **Semi2k.eqz_s(in)**
  Implementation of the equal to zero for share input under the Semi2k protocol.
  ##### **Parameters**
  * in - Share input
  ##### **Returns**
  * ArrayRef object of the result, $x = 0 \to 1, x \ne 0 \to 0$
  ***
  #### **Semi2k.bitdec_p(in, nbits)**
  Implementation of the bit decomposition for plain input under the Semi2k protocol.
  ##### **Parameters**
  * in - Plain input
  * nbits - Maximum decomposed binary bits
  ##### **Returns**
  * The decomposed vector of ArrayRef
  ***
  #### **Semi2k.bitdec_s(in, nbits)**
  Implementation of the bit decomposition for share input under the Semi2k protocol.
  ##### **Parameters**
  * in - Share input
  * nbits - Maximum decomposed binary bits
  ##### **Returns**
  * The decomposed vector of ArrayRef
  ***
  #### **Semi2k.h1bitdec_p(in, nbits)**
  Implementation of the highest bit decomposition for plain input under the Semi2k protocol.
  ##### **Parameters**
  * in - Plain input
  * nbits - Maximum decomposed binary bits
  ##### **Returns**
  * The decomposed vector of ArrayRef
  ***
  #### **Semi2k.h1bitdec_s(in, nbits)**
  Implementation of the highest bit decomposition for share input under the Semi2k protocol.
  ##### **Parameters**
  * in - Share input
  * nbits - Maximum decomposed binary bits
  ##### **Returns**
  * The decomposed vector of ArrayRef
  ***
  #### **Semi2k.matmul_pp(lhs, rhs, M, N, KK)**
  Implementation of matrix multiplication between plain matrix multiplier and plain matrix multiplier under the Semi2k protocol.
  ##### **Parameters**
  * lhs - First plain matrix multiplier
  * rhs - Second plain matrix multiplier
  ##### **Returns**
  * ArrayRef object of the matrix product, $lhs \cdot rhs$
  ***
  #### **Semi2k.matmul_sp(lhs, rhs, M, N, KK)**
  Implementation of matrix multiplication between share matrix multiplier and plain matrix multiplier under the Semi2k protocol.
  ##### **Parameters**
  * lhs - First share matrix multiplier
  * rhs - Second plain matrix multiplier
  ##### **Returns**
  * ArrayRef object of the matrix product, $lhs \cdot rhs$
  ***
  #### **Semi2k.matmul_ps(lhs, rhs, M, N, KK)**
  Implementation of matrix multiplication between plain matrix multiplier and share matrix multiplier under the Semi2k protocol.
  ##### **Parameters**
  * lhs - First plain matrix multiplier
  * rhs - Second share matrix multiplier
  ##### **Returns**
  * ArrayRef object of the matrix product, $lhs \cdot rhs$
  ***
  #### **Semi2k.matmul_ss(lhs, rhs, M, N, KK)**
  Implementation of matrix multiplication between share matrix multiplier and share matrix multiplier under the Semi2k protocol.
  ##### **Parameters**
  * lhs - First share matrix multiplier
  * rhs - Second share matrix multiplier
  ##### **Returns**
  * ArrayRef object of the matrix product, $lhs \cdot rhs$
  ***