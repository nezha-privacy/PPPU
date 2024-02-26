#pragma once

#include <type_traits>
#include <concepts>
#include <string>

#include "network/playerid.h"

namespace mpc
{

/// @struct isValidProtocolPlainType
/// @brief Check whether ptype is a valid plain type of protocol and each specific protocol do its own specilization.
/// @param protocol Protocol we use to check
/// @param ptype Plain data type to be checked
template <typename protocol, typename ptype>
struct isValidProtocolPlainType: std::false_type {};

/// @struct convertProtocolPlainTypeToShareType
/// @brief Convert plain type to corresponding share type and each specific protocol do its own specilization with member type "type".
/// @param protocol Protocol we use to convert
/// @param ptype Plain data type to be converteded
template <typename protocol, typename ptype>
struct convertProtocolPlainTypeToShareType;

/// @struct isValidProtocolPlainSharePair
/// @brief Check whether ptype and stype is match and each specific protocol do its own specilization.
/// @param protocol Protocol we use to check
/// @param ptype Plain data type to be checked
/// @param stype Share data type to be checked
template <typename protocol, typename ptype, typename stype>
struct isValidProtocolPlainSharePair: std::false_type {};

/// @class A protocol base class. Use to contains protocols.
class Protocol
{
public:
    Protocol() = default;
    virtual ~Protocol() = default;
    Protocol(Protocol const&) = default;
    Protocol& operator=(Protocol const&) = default;

    // member type
    // Preprocessing
    // pbit
    // pint[N]
};

/// @brief Judgment for instantiation.
template <typename T>
concept isProtocol = std::derived_from<T, Protocol>;

}
