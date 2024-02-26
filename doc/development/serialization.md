# 序列化

本序列化模块借鉴了 `boost archive` 的设计思路。

本着高效的设计思想，本模块的序列化操作几乎完全依赖于内存的直接拷贝，在这个过程中对序列化数据**不会**进行任何处理。

也就是说，使用本序列化模块的发送方和接收方的机器**必须**有着相同的大小端模式，并且双方对于序列化数据的类型以及先后顺序都**必须**在编译期达成共识。

本序列化模块支持序列化对象的类型如下：

* 布尔 `bool`
* 整型 `char`, `short`, `int`, `long`, `long long`
* 浮点 `float`, `double`
* 枚举 `enum`, `enum_class`
* 数组 `T[N]`, `T[N1]...[Nm]`
* 序列 `std::span`
* 容器 `std::array`, `std::vector`
* 自定义类型

序列化模块中提供了两个类，其中 `Serializer` 类负责将 C++ 对象转换为字节序列，`Deserializer` 类从字节序列中复原对象的信息。字节序列封装在 `ByteVector` 类中。

## `class Serializer`

在实例化 `Serializer` 对象时，应当使用默认的构造函数，构造函数会默默创建一个 `ByteVector` 类对象，序列化的所有数据会储存在其中。当需要发送序列化数据时，应当调用 `Serializer` 对象的 `finalize()` 方法，该方法返回字节序列的右值引用。

```C++
Serializer sr;
// do some serialization...
auto bytes = sr.finalize();   // ByteVector
```

`Serializer` 类提供三个接口，用于接收被序列化对象。

```C++
bool x;
int arr[5];

sr & x;                 // ok
sr << x;                // also ok
sr.serialize(x);        // also ok
```

`Serializer` 类的三个接口都可以连续接收被序列化对象。

```C++
sr &  x &  arr;                   // ok
sr << x << arr;                   // also ok
sr.serialize(x).serialize(arr);   // also ok
```

## `class Deserializer`

创建 `Deserializer` 对象时，构造函数接受的形参类型为 `ByteVector&&`。

```C++
ByteVector bytes;
Deserializer dr(std::move(bytes));
// do some deserialization...
```

在进行反序列化时，`Deserializer` 提供类似 `Serializer` 的接口，其功能一一对应：

* `operator&  (T&)`
* `operator>> (T&)`
* `deserialize(T&)`

对于可以默认构造的类型 `T`，可以调用 `Deserializer::get<T>()`，以返回值的形式获取一个类型 T 的实例。

## 自定义类

### 可平凡序列化

首先介绍一下 `concept TriviallySerializable`。平凡可序列化对象指的是可以通过直接对对象本身的内存进行拷贝即可完成序列化操作的对象。

基本类型中例如 `int`, `int[N]`, `float`, `enum` 等，都是可平凡序列化对象。若自定义类的数据成员也都是可平凡序列化的，则该自定义类对象也是可平凡序列化的。

对于可平凡序列化的自定义类，需要通过以下方式将其声明为可平凡序列化：

```C++
class MyClass {
public:
    static constexpr bool trivially_serializable = true;
};
```

### 自定义序列化方法

若自定义类 `T` 不是可平凡序列化的，则需要通过以下四种方式之一自定义该类的序列化方法：

* 定义序列化和反序列化方法
    * `template <typename AR> void MyClass::archive(AR&)`
* 分别定义序列化和反序列化方法
    * `void MyClass::serialize(Serializer&)`
    * `void MyClass::deserialize(Deserializer&)`
* 定义全局序列化和反序列化函数
    * `template <typename AR> void archive(AR&, MyClass&)`
* 分别定义全局序列化和反序列化函数
    * `void serialize(Serializer&, const MyClass&)`
    * `void deserialize(Deserializer&, MyClass&)`

通过 `&` 运算符可以将序列化和反序列化方法写在同一个函数中。例如，当模板参数被实例化为 `Serializer` 类时，则会对 `MyBuffer` 进行序列化操作；而当模板参数被实例化为 `Deserializer` 类时，则会对其进行反序列化操作。

```C++
class MyBuffer {
protected:
    char   buf[128];
    size_t len;

public:
    template <typename AR>
    void archive(AR& ar) {
        ar & len;
        ar & buf;
    }
};
```

## 数组和容器

对于静态数组，也就是数组大小在编译期确定的数组，若其元素类型是可序列化的，则数组本身也是可序列化的。

而对于指针以及动态数组，由于其大小在编译期不可知，因此不能直接对其进行序列化操作。因此本序列化模块提供了对 `std::span` 的支持：

* 在进行序列化操作时，只需要将指针或数组包装在 `std::span` 中，即可对齐进行序列化。但要注意的是，序列化器在对 `std::span` 进行序列化时，只会对数组中的元素进行序列化，而数组长度则需要用户自行进行序列化操作。
* 在进行反序列化操作时，用户首先需要自行获取数组长度（自行进行反序列化操作或通过其他方式），然后根据数组长度分配足够大小的内存，最后将分配好的内存包装在 `std::span` 中，传递给反序列化器。

```C++
class MyVector {
protected:
    int*   ptr;
    size_t len;

public:

    MyVector(): ptr(nullptr), len(0) {}
    ~MyVector() { delete[] ptr; }

    void serialize(Serializer& sr) const {
        sr << len;
        sr << std::span(ptr, len);
    }

    void deserialize(Deserializer& dr) {
        dr >> len;
        // 自行负责内存管理
        ptr = new int[len];
        dr >> std::span(ptr, len);
    }
};
```

对于标准库中容器，如 `std::array`, `std::vector` 等，本模块已经提供了预定义好的全局序列化和反序列化函数，只需要包含头文件 `stl.h` 即可。
