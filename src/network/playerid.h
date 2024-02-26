#pragma once

#include <cstdint>
#include <numeric>
#include <bitset>
#include <initializer_list>

using playerid_t = std::size_t;

/// @class mplayerid_t
/// @brief Be similar with std::set<playerid_t>.
class mplayerid_t {

public:

    static constexpr std::size_t MAX_NUM_PLAYERS = 128;
    
    using underlying_type = std::bitset<MAX_NUM_PLAYERS>;
    using size_type  = std::size_t;

protected:
    underlying_type _value;

    /// @brief Find the first player from std::bitset.
    /// @return The first player pid
    size_type _find_first() const;

    /// @brief Find the next player from std::bitset.
    /// @return The next player pid
    size_type _find_next(size_type) const;

    // explicit mplayerid_t(underlying_type);

public:

    mplayerid_t()                              = default;
    ~mplayerid_t()                             = default;
    mplayerid_t(mplayerid_t&&)                 = default;
    mplayerid_t(const mplayerid_t&)            = default;
    mplayerid_t& operator=(mplayerid_t&&)      = default;
    mplayerid_t& operator=(const mplayerid_t&) = default;

    mplayerid_t(std::initializer_list<playerid_t>);

    /// @brief Get all the players' mpid.
    /// @param n_players The number of players
    static mplayerid_t all(size_type n_players);

    /// @brief Get all the players' mpid except someone.
    /// @param n_players The number of players
    /// @param but Without someone
    static mplayerid_t all_but(size_type n_players, playerid_t but);

    /// @brief Determine whether the set is empty.
    /// @return If it is empty, return true
    bool      empty   () const;

    /// @brief Get the size of the set.
    /// @return The size of the set
    size_type size    () const;

    /// @brief Get the size of the set.
    /// @return The size of the set
    size_type max_size() const;

    /// @brief Clear the pid set of this object.
    void clear    ();

    /// @brief Insert a new pid into the set from input.
    /// @param playerid_t The new pid from input
    void insert   (playerid_t);

    /// @brief Erase a specific pid from the set.
    /// @param playerid_t The pid to be erased
    void erase    (playerid_t);

    /// @brief Swap two sets of this object and the input object.
    /// @param mplayerid_t The input mplayer pids object
    void swap     (mplayerid_t&);

    /// @brief Merge two sets of this object and the input object, the result stored in this object ( set union ).
    /// @param mplayerid_t The input mplayer pids object
    void merge    (const mplayerid_t&);

    /// @brief Determine whether this object's set contains specific pid from input.
    /// @param playerid_t The pid to be determined
    /// @return Whether the input pid is in this object's set
    bool contains (playerid_t) const;

    // union, symmetric difference, difference
    mplayerid_t& operator|= (const mplayerid_t&);
    mplayerid_t& operator^= (const mplayerid_t&);
    mplayerid_t& operator&= (const mplayerid_t&);

    // union, difference
    mplayerid_t& operator+= (const mplayerid_t&);
    mplayerid_t& operator-= (const mplayerid_t&);

    bool operator==(const mplayerid_t&) const;

    // range based for
    class iterator {
    protected:
        using size_type = std::size_t;

        const mplayerid_t&  _mpid;
        size_type           _pos;

    public:
        ~iterator()                          = default;
        iterator& operator=(const iterator&) = default;
        iterator(const mplayerid_t& mpid, size_type pos): _mpid(mpid), _pos(pos) {}

        iterator&  operator++()                      { _pos = _mpid._find_next(_pos); return *this; }
        bool       operator!=(const iterator& other) { return _pos != other._pos; }
        playerid_t operator* ()                      { return playerid_t(_pos); }
    };

    iterator begin() const;
    iterator end()   const;

};

// union, symmetric difference, difference
mplayerid_t operator| (const mplayerid_t&, const mplayerid_t&);
mplayerid_t operator& (const mplayerid_t&, const mplayerid_t&);
mplayerid_t operator^ (const mplayerid_t&, const mplayerid_t&);

// union, difference
mplayerid_t operator+ (const mplayerid_t&, const mplayerid_t&);
mplayerid_t operator- (const mplayerid_t&, const mplayerid_t&);
