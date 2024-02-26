
#include "playerid.h"

using size_type = mplayerid_t::size_type;
using iterator = mplayerid_t::iterator;

/************************ mplayer_t ************************/

/// @brief Find the first player from std::bitset.
/// @return The first player pid
size_type mplayerid_t::_find_first() const
{
    return _value._Find_first();
}

/// @brief Find the next player from std::bitset.
/// @return The next player pid
size_type mplayerid_t::_find_next(size_type pos) const
{
    return _value._Find_next(pos);
}

/// @brief Create a mplayer pids object from std::list.
/// @return The mplayer pids object
mplayerid_t::mplayerid_t(std::initializer_list<playerid_t> ilist)
{
    for (auto x : ilist) {
        this->insert(x);
    }
}

/// @brief Get all the players' mpid.
/// @param n_players The number of players
mplayerid_t mplayerid_t::all(size_type n_players)
{
    mplayerid_t ans;
    for (playerid_t i = 0; i < n_players; ++i) {
        ans.insert(i);
    }
    return ans;
}

/// @brief Get all the players' mpid except someone.
/// @param n_players The number of players
/// @param but Without someone
mplayerid_t mplayerid_t::all_but(size_type n_players, playerid_t but)
{
    auto ans = mplayerid_t::all(n_players);
    ans.erase(but);
    return ans;
}

/// @brief Determine whether the set is empty.
/// @return If it is empty, return true
bool mplayerid_t::empty() const
{
    return _value.none();
}

/// @brief Get the size of the set.
/// @return The size of the set
size_type mplayerid_t::size() const
{
    return _value.count();
}

/// @brief Get the size of the set.
/// @return The size of the set
size_type mplayerid_t::max_size() const
{
    return MAX_NUM_PLAYERS;
}

/// @brief Clear the pid set of this object.
void mplayerid_t::clear()
{
    _value.reset();
}

/// @brief Insert a new pid into the set from input.
/// @param playerid_t The new pid from input
void mplayerid_t::insert(playerid_t pid)
{
    _value.set(pid);
}

/// @brief Erase a specific pid from the set.
/// @param playerid_t The pid to be erased
void mplayerid_t::erase(playerid_t pid)
{
    _value.reset(pid);
}

/// @brief Swap two sets of this object and the input object.
/// @param mplayerid_t The input mplayer pids object
void mplayerid_t::swap(mplayerid_t &other)
{
    std::swap(_value, other._value);
}

/// @brief Determine whether this object's set contains specific pid from input.
/// @param playerid_t The pid to be determined
/// @return Whether the input pid is in this object's set
bool mplayerid_t::contains(playerid_t pid) const
{
    return _value.test(pid);
}

/// @brief Merge two sets of this object and the input object, the result stored in this object ( set union ).
/// @param mplayerid_t The input mplayer pids object
void mplayerid_t::merge(const mplayerid_t &other)
{
    this->_value |= other._value;
}

// set union
mplayerid_t &mplayerid_t::operator|=(const mplayerid_t &other)
{
    _value |= other._value;
    return *this;
}

mplayerid_t &mplayerid_t::operator^=(const mplayerid_t &other)
{
    _value ^= other._value;
    return *this;
}

// set intersection
mplayerid_t &mplayerid_t::operator&=(const mplayerid_t &other)
{
    _value &= other._value;
    return *this;
}

// set union
mplayerid_t &mplayerid_t::operator+=(const mplayerid_t &other)
{
    _value |= other._value;
    return *this;
}

// set different
mplayerid_t &mplayerid_t::operator-=(const mplayerid_t &other)
{
    _value &= (~other._value);
    return *this;
}

bool mplayerid_t::operator==(const mplayerid_t &other) const
{
    return _value == other._value;
}

iterator mplayerid_t::begin() const
{
    return iterator(*this, _find_first());
}

iterator mplayerid_t::end() const
{
    return iterator(*this, MAX_NUM_PLAYERS);
}


/************************ global operator ************************/


mplayerid_t operator|(const mplayerid_t &lhs, const mplayerid_t &rhs)
{
    mplayerid_t ans{lhs};
    ans |= rhs;
    return ans;
}

mplayerid_t operator&(const mplayerid_t &lhs, const mplayerid_t &rhs)
{
    mplayerid_t ans{lhs};
    ans &= rhs;
    return ans;
}

mplayerid_t operator^(const mplayerid_t &lhs, const mplayerid_t &rhs)
{
    mplayerid_t ans{lhs};
    ans ^= rhs;
    return ans;
}

mplayerid_t operator+(const mplayerid_t &lhs, const mplayerid_t &rhs)
{
    mplayerid_t ans{lhs};
    ans += rhs;
    return ans;
}

mplayerid_t operator-(const mplayerid_t &lhs, const mplayerid_t &rhs)
{
    mplayerid_t ans{lhs};
    ans -= rhs;
    return ans;
}
