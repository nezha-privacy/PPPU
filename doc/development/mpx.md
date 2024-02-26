# mpx

针对固定长度大整数的运算接口，基于 GMP 实现。

* mpx2k：$Z_{2^k}$ 运算
* mpxp：$Z_p$ 运算

## mpxp

所有的运算都在 $Z_{p}$ 上完成，其中 $p$ 是素数。
函数本身不进行内存分配，提供的输入和输出地址必须有足够存放 $N = \lceil \log_2 p \rceil$ 个比特的空间，通过模板参数指定 $N$ 的大小。

支持的运算

* 算术运算 `+`, `-`, `*`, `/`
* 比较运算 `<=>`



```C++
template <size_t N>
void mpxp_neg(
    mp_limb_t*       rp,
    const mp_limb_t* sp,
    const mp_limb_t* pp
);
```

$rp = (-sp) \mod pp$

```C++
template <size_t N>
void mpxp_add(
    mp_limb_t*       rp,
    const mp_limb_t* s1p,
    const mp_limb_t* s2p,
    const mp_limb_t* pp
);
```

$rp = (s1p + s2p) \mod pp$

```C++
template <size_t N>
void mpxp_sub(
    mp_limb_t*       rp,
    const mp_limb_t* s1p,
    const mp_limb_t* s2p,
    const mp_limb_t* pp
);
```

$rp = (s1p - s2p) \mod pp$

```C++
template <size_t N>
void mpxp_mul(
    mp_limb_t*       rp,
    const mp_limb_t* s1p,
    const mp_limb_t* s2p,
    const mp_limb_t* pp
);
```

$rp = (s1p \times s2p) \mod pp$


```C++
template <size_t N>
void mpxp_sqr(
    mp_limb_t*       rp,
    const mp_limb_t* sp,
    const mp_limb_t* pp
);
```

$rp = sp^2 \mod pp$

```C++
template <size_t N>
void mpxp_inv(
    mp_limb_t*       rp,
    const mp_limb_t* sp,
    const mp_limb_t* pp
);
```

$rp = sp^{-1} \mod pp$

```C++
template <size_t N>
void mpxp_div(
    mp_limb_t*       rp,
    const mp_limb_t* s1p,
    const mp_limb_t* s2p,
    const mp_limb_t* pp
);
```

$rp = (s1p / s2p) \mod pp$

```C++
template <size_t N>
int mpxp_cmp(
    const mp_limb_t* s1p,
    const mp_limb_t* s2p,
    const mp_limb_t* pp
);
```

比较 `s1p` 和 `s2p`，若相等则返回 0；若 `s1p > s2p`，则返回 `+1`；若 `s1p < s2p`，则返回 `-1`。

## mpx2k

所有的运算都在 $Z_{2^k}$ 上完成。
函数本身不进行内存分配，提供的输入和输出地址必须有足够存放 $k$ 个比特的空间，通过模板参数指定 $k$ 的大小。

支持的运算

* 算术运算 `+`, `-`, `*`
* 位运算 `&`, `|`, `^`, `~`, `<<`, `>>`
* 比较运算 `<=>`

```C++
template <size_t K>
void mpx2k_neg(
    mp_limb_t*       rp,
    const mp_limb_t* sp
);
```

$rp = (-sp) \mod 2^k$

```C++
template <size_t K>
void mpx2k_add(
    mp_limb_t*       rp,
    const mp_limb_t* s1p,
    const mp_limb_t* s2p
);
```

$rp = (s1p + s2p) \mod 2^k$

```C++
template <size_t K>
void mpx2k_sub(
    mp_limb_t*       rp,
    const mp_limb_t* s1p,
    const mp_limb_t* s2p
);
```

$rp = (s1p - s2p) \mod 2^k$

```C++
template <size_t K>
void mpx2k_mul(
    mp_limb_t*       rp,
    const mp_limb_t* s1p,
    const mp_limb_t* s2p
);
```

$rp = (s1p \times s2p) \mod 2^k$

```
template <size_t K>
void mpx2k_and(
    mp_limb_t*       rp,
    const mp_limb_t* s1p,
    const mp_limb_t* s2p
);
```

Bitwise AND

```
template <size_t K>
void mpx2k_ior(
    mp_limb_t*       rp,
    const mp_limb_t* s1p,
    const mp_limb_t* s2p
);
```

Bitwise Inclusive OR

```C++
template <size_t K>
void mpx2k_xor(
    mp_limb_t*       rp,
    const mp_limb_t* s1p,
    const mp_limb_t* s2p
);
```

Bitwise Exclusive OR

```C++
template <size_t K>
void mpx2k_com(
    mp_limb_t*       rp,
    const mp_limb_t* sp
);
```

Two's Complement

```C++
template <size_t K>
void mpx2k_lshift(
    mp_limb_t*       rp,
    const mp_limb_t* sp,
    size_t           cnt
);
```

左移 cnt 位。

```C++
template <size_t K>
void mpx2k_rshift(
    mp_limb_t*       rp,
    const mp_limb_t* sp,
    size_t           cnt
);
```

右移 cnt 位。


```C++
template <size_t K>
int mpx2k_cmp(
    const mp_limb_t* s1p,
    const mp_limb_t* s2p
);
```

比较 `s1p` 和 `s2p` 的大小，若相等则返回 0；若 $s1p > s2p$，则返回 $1$；若 $s1p < s2p$，则返回 $-1$。




