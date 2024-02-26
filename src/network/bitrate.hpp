#pragma once

#include "bitrate.h"

namespace network {

/************************ Bitrate ************************/

template <typename V, typename M>
constexpr bool Bitrate<V, M>::operator==(Bitrate<V, M> other) const {
    return _count == other._count;
}

template <typename V, typename M>
constexpr auto Bitrate<V, M>::operator<=>(Bitrate<V, M> other) const {
    return _count <=> other._count;
}

/// @brief Use to print bit rate object.
/// @return The content of this bit rate object
template <typename V, typename M>
std::string Bitrate<V, M>::to_string() const {
    if ( _count == 0 )           return std::string("0bps");
    if ( *this == unlimited() )  return std::string("unlimited");

    auto str = std::to_string(_count);
    if constexpr ( std::same_as<M, std::kilo> ) {
        str += "K";
    } else if constexpr ( std::same_as<M, std::mega> ) {
        str += "M";
    } else if constexpr ( std::same_as<M, std::giga> ) {
        str += "G";
    } else {
        throw std::runtime_error("unknown bitrate M");
    }

    str += "bps";
    return str;
}

/// @brief Bitrate cast.
template <typename V1, typename M1>
template <typename V2, typename M2>
requires (!std::same_as<V1, V2> || !std::same_as<M1, M2>)
constexpr Bitrate<V1, M1>::Bitrate(Bitrate<V2, M2> other) {
    if( other == decltype(other)::unlimited() ) {
        *this = this->unlimited();
    } else {
        using M3 = std::ratio_divide<M2, M1>;
        _count = (
            static_cast<decltype(_count)>( other.count() )
            * M3::num / M3::den
        );
    }
}

/************************ Datasize ************************/

/// @brief Use to print data size object.
/// @return The content of data size rate object
template <typename V, typename M>
std::string Datasize<V, M>::to_string() const {
    
    std::string number  = std::to_string(_count);
    std::string suffix;

    if constexpr ( std::same_as<M, std::ratio<1, 1>> ) {
        suffix = "B";
    } else if constexpr ( std::same_as<M, std::kilo> ) {
        suffix = "KB";
    } else if constexpr ( std::same_as<M, std::mega> ) {
        suffix = "MB";
    } else if constexpr ( std::same_as<M, std::giga> ) {
        suffix = "GB";
    } else {
        throw std::runtime_error("unknown multiple of datasize");
    }

    return number + suffix;
}

template <typename V, typename M>
constexpr Datasize<V, M> Datasize<V, M>::operator+(Datasize<V, M> other) const {
    return Datasize<V, M>(_count + other._count);
}

template <typename V, typename M>
constexpr Datasize<V, M> Datasize<V, M>::operator-(Datasize<V, M> other) const {
    assert(_count >= other._count);
    return Datasize<V, M>(_count - other._count);
}

template <typename V, typename M>
constexpr Datasize<V, M> Datasize<V, M>::operator*(unsigned long long multiple) const {
    return Datasize<V, M>(_count * multiple);
}

template <typename V, typename M>
constexpr Datasize<V, M>& Datasize<V, M>::operator+=(Datasize<V, M> other) {
    _count += other._count;
    return *this;
}

template <typename V, typename M>
constexpr Datasize<V, M>& Datasize<V, M>::operator-=(Datasize<V, M> other) {
    assert(_count >= other._count);
    _count -= other._count;
    return *this;
}

template <typename V, typename M>
constexpr Datasize<V, M>& Datasize<V, M>::operator*=(unsigned long long multiple) {
    _count *= multiple;
    return *this;
}

template <typename V, typename M>
constexpr bool Datasize<V, M>::operator==(Datasize<V, M> other) const {
    return _count == other._count;
}

template <typename V, typename M>
constexpr auto Datasize<V, M>::operator<=>(Datasize<V, M> other) const {
    return _count <=> other._count;
}

/// @brief Datasize cast.
template <typename V1, typename M1>
template <typename V2, typename M2>
requires (!std::same_as<V1, V2> || !std::same_as<M1, M2>)
constexpr Datasize<V1, M1>::Datasize(Datasize<V2, M2> other) {
    if( other == decltype(other)::infinity() ) {
        *this = this->infinity();
    } else {
        using M3 = std::ratio_divide<M2, M1>;
        _count = (
            static_cast<decltype(_count)>( other.count() )
            * M3::num / M3::den
        );
    }
}

/******************************** arithmetic ********************************/

/// @brief Datasize = Duration * Bitrate.
template <typename V1, typename V2, std_ratio M1, std_ratio M2>
constexpr auto operator* (
    std::chrono::duration<V1, M1> duration,
    Bitrate<V2, M2> bitrate
) {

    if( bitrate == decltype(bitrate)::unlimited() ) {
        throw std::runtime_error("duration muliplied by unlimited bitrate");
    }

    using ValueType = decltype(duration.count() * bitrate.count() / 8);
    ValueType datasize_count = duration.count() * bitrate.count() / 8;
    using Multiple = std::ratio_multiply<M1, M2>;
    return Datasize<ValueType, Multiple>(datasize_count);
}

/// @brief Datasize = Bitrate * Duration.
template <typename V1, typename V2, std_ratio M1, std_ratio M2>
constexpr auto operator* (
    Bitrate<V1, M1> bitrate,
    std::chrono::duration<V2, M2> duration
) {
    return duration * bitrate;
}

/// @brief Duration = Datasize / Bitrate.
template <typename V1, typename V2, std_ratio M1, std_ratio M2>
constexpr auto operator/ (
    Datasize<V1, M1> datasize,
    Bitrate<V2, M2> bitrate
) {

    if(bitrate == decltype(bitrate)::unlimited()) {
        throw std::runtime_error("datasize divide by unlimited bitrate");
    } else if(bitrate == decltype(bitrate)::zero() ) {
        throw std::runtime_error("datasize divide by zero bitrate");
    }

    auto duration_count = 8 * datasize.count() / bitrate.count();

    using Multiple = std::ratio_divide<M1, M2>;
    using ValueType = decltype(duration_count);
    return std::chrono::duration<ValueType, Multiple>(duration_count);
}

/// @brief Bitrate = Datasize / duration.
template <typename V1, typename V2, std_ratio M1, std_ratio M2>
constexpr auto operator/ (
    Datasize<V1, M1> datasize,
    std::chrono::duration<V2, M2> duration
) {
    using ValueType = decltype(8 * datasize.count() / duration.count());
    using Multiple = std::ratio_divide<M1, M2>;
    using BitrateType = Bitrate<ValueType, Multiple>;
    
    if(duration.count() == 0) {
        return BitrateType::unlimited();
    } else {
        return BitrateType( 8 * datasize.count() / duration.count() );
    }
}

} // namespace network
