# NDArrayRef

## 工厂函数

* arange(start, stop, step=1) -> NDArrayRef<dtype>
  返回一个一维数组，第 i 个元素的值为 start + step*i
* zeros(shape) -> NDArrayRef<dtype>
  返回给定形状的数组，元素的值均为 0
* ones(shape) -> NDArrayRef<dtype>
  返回给定形状的数组，元素的值均为 1

## 基本操作

* `ndim() -> int64_t`
  获取数组的维度
* `numel() -> int64_t`
  获取数组的元素数量
* `shape(i) -> int64_t`
  获取第 i 维的元素数量（等价于 `shape()[i]`）
* `shape() -> std::vector<int64_t> const&`
  获取数组的形状
* `data() -> dtype*`
  获取底层数据访问。注意指针指向的位置并不一定是数组的第一个元素。

* `copy() -> NDArrayRef<dtype>`
  对数组进行复制。复制后的数组将拥有独立的内存。
* `transpose() -> NDArrayRef<dtype>`
  对数组进行转置。例如，形状为 M*N 的数组转置后的形状为 N*M。
* `as<new_type>() -> NDArrayRef<new_type>`
  在不改变元素值的前提下，进行类型转换。
* `reshape(new_shape) -> NDArrayRef<dtype>`
  在不改变元素排列顺序的前提下，改变数组的形状。
* `slice(indicies) -> NDArrayRef<dtype>`
  对数组进行切片
* `elem(index) -> dtype&`
  获取位于 index 处的元素。

## 运算

TODO

* reduce
* element wise operation
* matrix multiplication

## 例

```C++
auto a = zeros<double>({3, 4});
auto b = arange<int>(0, 16).reshape({4, 4});

assert(a.ndim() == 2);
assert(a.numel() == 12);
assert(a.shape(0) == 3);
assert(a.shape(1) == 4);

assert(b.elem({0, 0}) == 0);
assert(b.elem({1, 1}) == 5);
assert(b.elem({2, 2}) == 10);
assert(b.elem({3, 3}) == 15);

auto s1 = b.slice({ Slice{.stop=3}, {} });  // b[:3:, ::]
auto s2 = b.slice({ 1, Slice{.step=-1} });  // b[1, ::-1]

assert(s1.shape() == a.shape());
assert(s2.ndim() == 1);

auto d = matmul(a.as<int>(), s1.transpose());
```
