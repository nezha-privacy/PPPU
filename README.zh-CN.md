# PPPU
**其他语言版本: [English](README.md), [中文](README.zh-CN.md).**

## 简介
一个开源隐私计算单元，目前提供semi2k协议下基础计算，定点数计算等。

## 安装
### 预安装
* GNU Multi Precision 6.2.1 (built with C++ support)
* Boost 1.80
* OPENSSL (1.1.1 经过验证)
* github.com/fmtlib/fmt (9.1.0 经过验证)
* relic (for OT RCurve)
* google benchmark & googletest (for network test and context benchmark)

### 安装步骤
```
$ git clone https://github.com/qazw52/PPPU.git
$ unzip PPPU.zip
$ cd PPPU

编辑 CMakeLists.txt 并修改你自己的安装路径 CMAKE_CXX_COMPILER(应为最新), boost, fmt, Google benchmark, 和 Google test 在 CMakeLists.txt 中。

$ mkdir build
$ cd build
$ cmake ..
$ make

目前，可执行文件位于 build 文件夹中。
```

### 快速开始
* 运行测试
```
$ cd build
$ ./tutorial
```
## 层次
### 纵向结构
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

### Data Layer（数据层）
#### Context（上下文）
* 包含一个 **Value** 对象，该对象封装 MPC 计算所需的所有元素，包括 MPC 协议、非共享数据类型和共享数据类型。
* 每个Value对象只能处于三种隐私状态（**public, private, share**，前两种统称为 **plain**），并且每个隐私状态可以具有在不同 MPC 协议下存储的不同数据类型。
* **Basic** —— 实现了算术运算，包括输入和输出、按位分解、加法、减法、乘法等，接口在 **basic.h**中。
  * 用于生成 **Value** 对象的工厂函数接口位于 **factory.h**。
  * 对定点数进行算术运算的特殊实现位于 **fxp.h** 中。
  * 基于隐私条件和协议的基本函数和分发函数分别位于 **raw.h** 、**primity.h** 和 **prot_wrapper.h** 。
  * 编码为 Value 和解码为 NDArray 函数接口位于 **util.h** 中。
* **Compare** —— 实现了较小和较大等比较操作，接口位于 **compare.h** 中。
* **Math** —— 实现了 **division, exponential, logarithmic, polynomial, power, round to, sigmoid, sqrt** 等数学运算，接口在相应的头文件中。
* **Shape** —— 实现了形状更改操作，如 **concatenate, reduce, sort**，接口在相应的头文件中。

#### NDArray（N维数组）
* 包含一个实现 n 维数组及其计算的 **NDArray** 对象，接口位于 **NDArray_ref.h** 中。
* 一维数组的实现在 **array_ref.h** 中。数据存储在 **buffer.h** 中。
* 其他头文件是功能模块，如 **concatenate, iterator, packbits, serialization, slice** ，接口在相应的头文件中。

#### Datatype（数据类型）
* 包含 **Z2k、Zp、FixedPoint** 对象，这些对象实现了各种数据类型及其相应的操作，接口位于相应的头文件中。
* 上述数据类型的操作的实现存储在 **mpx##.h** 中，其基于 gmp。
  * **Z2k** —— 一个模为2^K的有限域，提供了基本运算和有限域上的运算，接口在 **Z2k.h** 中。
  * **Zp** —— 一个有限域，其模为素数，提供了基本运算和有限域上的运算，接口在 **Zp.h** 中。
  * **FixedPoint** —— 基于Z2k，一个定点数的实现，并提供了基本操作，接口在 **fix_point.h** 中。

### Communication Layer（交流层）
#### Network（网络）
* 包含一个 **MultiPartyPlayer** 对象，用于实现 n 个参与方之间的通信。还提供了一个在两个参与方之间使用的 **TwoPartyPlayer** 对象。
* MultiPartyPlayer 和 TwoPartyPlayer 都有 Secure 和 Plain 版本，分别使用 SSL 和 TCP 作为其套接字，分别在 **multi_party_player.h** 和 **two_party_player.h** 中包含计算中使用的真实接口。
* 连接函数在 **mp_connect.h** 中，套接字在 **socket_package.h** 中，参与方 id 在 **playerid.h** 中，同时通信信息在 **statistics.h** 中。其他头文件包含一些实现。

#### Serialization（序列化）
* 包含通信中使用的 **Serializer** 和 **Deserializer** 。序列化程序将 ArrayRef 转换为 ByteVector，反序列化程序将 ByteVector 转换为 ArrayRef。ByteVector用于通信。接口位于 **serializer.h** 和 **deserializer.h** 中。

### Function Layer（功能层）
#### ML（机器学习）
* 基于PPPU的机器学习功能。

#### MPC（多方安全计算）
* 包含计算中使用的 MPC 协议。接口在 **protocol/procol.hpp** 中。要使用协议，我们需要 Protocol 对象和 Preprocessing 对象来定义协议，接口在 **Protocol.hpp** 和 **Preprocessing.hpp** 中。
* 目前我们实现了 **Semi2k** 协议。

#### PSI（隐私集合求交）
* 包含 **PSI**（隐私集合交集）函数，现在有两种算法（**crypto20，EcdhPSI**）。接口位于 **algorithm/PSISender.h** 和 **algorithm/PISReceiver.h** 中。

#### OT（不经意传输）
* 包含基本的 **OT**（不经意传输）函数。接口位于 **OTInterface.h** 和 **BasicDefinitions.h** 中。

### Other Layers（其他层）
#### Tools（工具）
* 包含上述层中使用的数学运算和数据结构。

#### Config（设置）
* 用于读取基于 boost programs options 的网络配置。

## 协议
### Semi2k-SPDZ
  类似于SPDZ的半诚实 MPC 协议，但需要可信的第三方来生成离线随机数。默认情况下，此协议现在使用受信任的第一方。因此，它应该仅用于调试目的。

## 接口
  有关API的具体说明，请参阅具体文件中的注释。这里只是函数名，至于参数和返回值，请参考原始文件。

### Context

#### value.h
* **class Value<Protocol, pdtype, sdtype>**：PPPU中的主要计算单位，基于NDArray。
* **fracbits() + set_fracbits()**：**frackbits** 的 Getter 和 Setter，表示一个定点数的小数点位置，从右往左数。
* **visibility() + set_visibility()**：**vis** 的 Getter 和 Setter，表示此值的可见性类型。
* **is_public() + is_private() + is_plain() + is_share()**：判断可见性。
* **owner()**：获取 Value 的所有者。
* **data_p/s() + assign_p/s()**：**data** 的 Getter 和 Setter。
* **ndim(), numel(), offset(), shape(), strides()**：获取 **data** 的 NDArray 的属性。
* **copy(), slice(), reshape(), transpose(), broadcast_to(), permute(), substitute()**：设置 **data** 的 NDArray 的属性，通过不同方式进行更改。

#### visibility.h
* **class Visibility**：表示值的公开状态。(Op: =)
* **type**：**VIS_PUBLIC** - 各方都知道, **VIS_PRIVATE** - 只有一方知道, **VIS_SHARE** - 各方共享的秘密。
* **is_public(), is_private(), is_share()**：**type** 的 Getter.
* **Public(), Private(), Share()**：**type** 的 Setter.
* **owner()**: 获取 Visibility 对象的所有者。

#### basic/basic.h
* **input()**：将私有值作为分享值输入。
* **open()**：将分享值公开为公开值。
* **neg()**：获取输入Value数组中元素的相反数。
* **add()**：按位相加两个 Value。
* **sub()**：按位相减两个 Value。
* **mul()**：按位相乘两个 Value。
* **square()**：按位获取输入 Value 的平方。
* **matmul()**：按位相乘两个矩阵 Value。
* **msb()**：获取输入 Value 的最高有效位。
* **eqz()**：逐元素判断是否为 0，如果为真，则将相应的位设置为 1。
* **sign()**：获取输入 Value 的符号位。
* **abs()**：获取输入 Value 的数组中元素的绝对值。
* **bitdec()**：将输入 Value 中的十进制值分解为存储在 Value 向量中的二进制位。
* **h1bitdec()**：将输入 Value 中的十进制值分解为存储在 Value 向量中的二进制位。只记录向量中的最高位，其他位设置为 0。
* **bitcomp()**：将位分解后的 Value 数组合为十进制值。

#### basic/factory.h
* **make_public()**：基于 NDArray 或 Value 创建一个公共 Value 对象（如果它是浮点类型，请将 fracbits 设置为 -1）。
* **make_private()**：基于 NDArray 或 Value 或其他方创建私有 Value 对象（如果是浮点类型，请将 fracbits 设置为-1，不要忘记设置所有者）。

#### basic/util.h
* **encode()**：用于将向量转换为内置类型。
* **decode()**：用于将内置类型转换为向量。（打印数据时使用。）
  
#### compare/compare.h
* **logical_not()**：执行逻辑运算否。
* **logical_and()**：执行逻辑操作和。
* **logical_or()**：执行逻辑操作或。
* **conditional()**：用于获取两个 Value 的两个互斥部分。
* **equal_to()**：判断一个 Value 中对应位置的值和另一个是否相等。
* **not_equal_to()**：判断一个 Value 中对应位置的值和另一个是否不相等。
* **less()**：获取两个输入 Value 中对应位置较小的指示向量。
* **greater()**：获取两个输入 Value 中对应位置较大的指示向量。
* **less_equal()**：获取两个输入 Value 中对应位置较小或相等的指示向量。
* **greater_equal()**：获取两个输入 Value 中对应位置较大或相等的指示向量。
* **min()**：获取两个输入 Value 中对应位置较小的值。
* **max()**：获取两个输入 Value 中对应位置较大的值。

#### math/div.h
* **div()**: 执行除法运算 a / b.
* **reciprocal()**：获取输入 Value 的倒数。

#### math/exp.h
* **exp()**：获取自然对数的基数的 x 次方。
* **exp2()**：获取 2 的 x 幂。

#### math/log.h
* **log()**：获取输入数据的自然对数。
* **log2()**：获取输入数据的以 2 为底的对数。
* **log10()**：获取输入数据的以 10 为底的对数。

#### math/polynomial.h
* **polynomial()**：给定 x，得到特定多项式的值。

#### math/pow.h
* **pow()**：获取 x 值的 y 次方。

#### math/round.h
* **fraction()**：记录输入值 x 的小数位数。
* **floor()**：将数字舍入为整数（向下取整）。
* **ceil()**：将数字舍入为整数（向上取整）。
* **round()**：将数字舍入为整数（四舍五入）。
* **mod()**：对值 x 执行模运算。

#### math/sigmoid.h
* **sigmoid()**：获取输入 Value 的 sigmoid 函数。

#### math/sqrt.h
* **sqrt()**：获取输入 Value 的平方根。

#### shape/concatenate.h
* **concatenate()**：端到端连接一组 Value。

#### shape/reduce.h
* **sum()**：求和输入 Value 数组中的所有元素。
* **min()**：获取输入 Value 数组中所有元素的最小值。（compare.h 中的 min() 具有不同的参数。）
* **max()**：获取输入 Value 数组中所有元素的最大值。（compare.h 中的 max() 具有不同的参数。）
* **argmax()**：查找最大值及其在输入 Value 数组中的相应位置。

#### shape/sort.h
* **sort()**：对 Value 数组执行奇偶合并排序。

### NDArray

#### ndarray_ref.h
* **class NDArrayRef**：NDArrayRef 类提供了一个访问和操作数组的接口，包括数据指针、元素数量、步长、偏移量等。它还定义了迭代器类型和相关的运算符重载，以便轻松遍历和访问数组中的元素。此外，它还提供了一些辅助方法，如类型转换、切片操作等。
* **ndim(), numel(), offset(), shape(), strides()**：获取此 NDArray 的属性。
* **as(), compact(), copy(), slice(), reshape(), transpose(), broadcast_to(), permute(), substitute()**：设置此 NDArray 的属性，通过不同的方式更改它们。
* **elem()**：返回对指定索引位置处元素的引用。
* **sptr()**：返回一个指向缓冲区的指针。
* **data()**：返回指向元素的连续内存区域的指针。
* **begin(), end(), lbegin(), lend()**：通用（或线性）迭代器（到普通或常量元素），始终有效。
* **make_ndarray()**：使 NArrayRef 为空，或填充值，std:：vector＜dtype＞ 或 std:：initializer_list＜dttype＞。
* **arange()**：生成一个 NDArrayRef 对象，该对象包含指定范围内的一系列值。
* **zeros(), ones()**：使 NDArrayRef 填充 0 或 1。

### Datatype

#### Z2k.h
* **class Z2<K, Signed>**：“Z2”类表示模为 2^K 的有限域，并提供对有限域的基本运算和运算。K 是表示模 2^K 的位数。Signed 用于确定 Z2 是有符号数还是无符号数。(Op: +-*/, ~&|^, <<>>, +=, -=, *=, &=, |=, ^=, <<=, >>=, ==, =)
* **class Zp<size_t N_BITS>**：函数小于 Z2k，单位为 Zp.h。
* **Z2()**：为方便起见，允许从内置类型进行隐式转换，或从浮点类型、不同位数或不同符号、mpz_class 类型值或字符串和基数（默认十进制）进行显式转换。
* **double(), to_string(), to_mpz()**：将有限字段元素转换为双类型表示法、字符串表示法（默认十进制）或 mpz_class 类型表示法。
* **assign()**：将整型、浮点型、mpz_class 类型、字符串和基数（默认十进制）或不同位数或不同符号的值分配给有限字段元素。
* **msb()**：获取 _data 中最高有效位置的位的值。
* **bit()**：获取 _data 中指定位置的位的值。
* **data()**：返回基础数据的指针。

### Network

#### multi_party_player.h
* **two_party_player.h** 与此模块类似。
* **class MultiPartyPlayer**：多方通信条件下的参与方实体。它是一个具有基本网络接口的基类。
* **class SecureMultiPartyPlayer**：使用 SSL 套接字作为其套接字的 MultiPartyPlayer。
* **class PlainMultiPartyPlayer**：使用 TCP 套接字作为其套接字的 MultiPartyPlayer。
* **run()**：运行所有参与方的线程。
* **connect()**：将端点互连在一起。
* **stop()**：停止正在运行的线程。
* **setup_ssl_context()**：仅在 SecureMultiPartyPlayer 中用于设置 SSL 协议的上下文。
* **id(), all(), all_but_me(), num_players()**：返回我的参与方 id，所有参与方的 mpid，除了自己的以外的所有参与方的 mpid，返回参与方数量。
* **sync()**：清除我的缓冲区并与所有其他参与方同步。
* **send()**：向其他参与方发送消息。
* **msend()**：分别向其他参与方发送不同的消息。
* **recv**：接收来自其他参与方的消息。
* **mrecv**：分别接收来自其他参与方的不同消息。
* **broadcast()**：向所有其他参与方广播消息。
* **mbroadcast()**：向一组参与方广播相同的消息。
* **exchange()**：向发送消息，然后从其他参与方处接收消息。
* **pass_around()**：向下一位参与方发送消息，并接收上一位参与方的消息。
* **broadcast_recv()**：广播消息，然后接收来自所有其他参与方的消息。
* **mbroadcast_recv()**：广播消息，然后在一组参与方之间接收。
* **is_running()**：判断线程是否正在运行。
* **get_statistics()**：获取网络统计信息。

#### playerid.h
* **playerid_t**: playerid_t = std::size_t.

### MPC

#### protocol.hpp
* **class Protocol**：协议基类。用于包含协议。

#### preprocessing.hpp
* **class Preprocessing**：预处理基类。用于包含预处理。

#### semi2k/semi2k.hpp
* **class Semi2k**：类似于 SPDZ 的半诚实 MPC 协议，但需要可信的第三方来生成离线随机数。
* **class Semi2kTripl**：Semi2k 协议中使用的乘法三元组。至于值得信赖的第三方。
* **plain(public, private), share**：对于函数名称中的 **p，s**，表示计算在可见性之间或可见性上。
* **input_p()**：远程输入或本地专用输入，用于来自 pid 的纯输入。
* **open_s()**：向各方开放共享以进行共享输入。
* **neg_p/s()**：p/s 输入为负值。
* **add_pp/sp/ss()**：p/s/s 加数和 p/p/s 加数之间的加法。
* **mul_pp/sp/ss()**：p/s/s 乘数和 p/p/s 乘数之间的乘积。
* **matmul_pp/sp/ss()**：p/s/s 矩阵乘子和 p/p/s 矩阵乘子之间的矩阵乘法。
* **msb_p/s()**：获取 p/s 输入的最高有效位。
* **lshift_p/s()**：p/s 输入的左移位。
* **trunc_p/s()**：p/s 输入的截断。
* **eqz_p/s()**：对于 p/s 输入，等于零。
* **bitdec_p/s()**：p/s 输入的位分解。
* **h1bitdec_p/s()**：p/s 输入的最高位分解。
