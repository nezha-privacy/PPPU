#pragma once

#include "network/playerid.h"

namespace pppu
{

/// @class Visibility
/// @brief Indicates the public status of the Value.
class Visibility
{
protected:

    enum {
        VIS_PUBLIC,    // known to all parties
        VIS_PRIVATE,   // known to only one party
        VIS_SHARE,     // secret shared by all parties
        VIS_INVALID
    } _type;

    playerid_t _owner; // only valid when private

    Visibility(decltype(_type) type, playerid_t owner): _type(type), _owner(owner) {}

public:

    static Visibility Public() { return Visibility(VIS_PUBLIC, -1); }
    static Visibility Share()  { return Visibility(VIS_SHARE,  -1); }
    static Visibility Private(playerid_t owner) { return Visibility(VIS_PRIVATE, owner); }

    Visibility(Visibility const&)            = default;
    Visibility& operator=(Visibility const&) = default;
    ~Visibility()                            = default;

    Visibility(): _type(VIS_INVALID), _owner(-1) {}

    /// @brief Determine the visibility of value as public.
    /// @return If it is public, return true, otherwise return false
    bool is_public () const { return _type == VIS_PUBLIC;  }

    /// @brief Determine the visibility of value as private.
    /// @return If it is private, return true, otherwise return false
    bool is_private() const { return _type == VIS_PRIVATE; }

    /// @brief Determine the visibility of value as share.
    /// @return If it is share, return true, otherwise return false
    bool is_share  () const { return _type == VIS_SHARE;   }

    /// @brief Determine the visibility of value as invalid.
    /// @return If it is invalid, return true, otherwise return false
    bool is_invalid() const { return _type == VIS_INVALID; }

    /// @brief Get the owner of this Value.
    /// @return The owner's player id
    playerid_t owner() const { return _owner; }

    bool operator==(Visibility other) const { return this->_type == other._type && this->_owner == other._owner; }
};

} // namespace pppu
