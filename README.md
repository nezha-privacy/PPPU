# PPPU
一个开源隐私处理单元，目前提供半诚实的SPDZ2k协议下的基本计算和定点数字计算，该项目目前处于测试阶段。  
**其他语言版本: [English](README.md), [中文](README-CN.md).**

## 更新日志
新增动态链接库和头文件安装。之后准备把fmt依赖删除掉，报错太多了。

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
$ git clone https://github.com/nezha-privacy/PPPU.git
$ unzip PPPU.zip
$ cd PPPU

尝试使用 install.sh 安装预安装内容。如果已经安装一些预安装内容，请修改CMakeLists.txt里的路径。

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


