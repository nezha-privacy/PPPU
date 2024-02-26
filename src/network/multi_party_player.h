#pragma once

#include "bitrate.hpp"
#include "comm_package.h"
#include "playerid.h"
#include "socket_package.h"
#include "statistics.h"

#include "../tools/byte_vector.h"
#include "../tools/timer.h"

namespace network
{

/************************ multi party player ************************/

/// @class MultiPartyPlayer
/// @brief Player entity under multi party communication conditions. It is a base class with basic network interface.
class MultiPartyPlayer
{

  public:
    using size_type = std::size_t;
    using offset_type = int;

  protected:
    playerid_t _my_pid;
    size_type  _n_players;
    Timer      _timer;

  protected:

    virtual void impl_sync() = 0;

    virtual void        impl_send           ( playerid_t to,      ByteVector && message ) = 0;
    virtual ByteVector  impl_recv           ( playerid_t from,    size_type size_hint   ) = 0;
    virtual ByteVector  impl_exchange       ( playerid_t peer,    ByteVector && message ) = 0;
    virtual ByteVector  impl_pass_around    (offset_type offset,  ByteVector && message ) = 0;
    virtual mByteVector impl_broadcast_recv (                     ByteVector && message ) = 0;

    virtual void        impl_broadcast      (                     ByteVector && message ) = 0;
    virtual void        impl_msend          (mplayerid_t tos,    mByteVector && messages) = 0;
    virtual mByteVector impl_mrecv          (mplayerid_t froms,   size_type size_hint   ) = 0;
    virtual void        impl_mbroadcast     (mplayerid_t to,      ByteVector && messages) = 0;
    virtual mByteVector impl_mbroadcast_recv(mplayerid_t group,   ByteVector && message ) = 0;

  public:
    virtual ~MultiPartyPlayer()                           = default;
    MultiPartyPlayer(MultiPartyPlayer &&)                 = default;
    MultiPartyPlayer(MultiPartyPlayer const &)            = delete;
    MultiPartyPlayer &operator=(MultiPartyPlayer &&)      = default;
    MultiPartyPlayer &operator=(MultiPartyPlayer const &) = delete;

    /// @brief Constructor.
    /// @param my_pid Player's id used to construct
    /// @param n_players Number of players
    MultiPartyPlayer(playerid_t my_pid, size_type n_players)
        : _my_pid(my_pid), _n_players(n_players) {}

    /// @brief Return my player id.
    /// @return My player id.
    playerid_t  id()         const;

    /// @brief Return all players' mpid.
    /// @return All players' mpid including me
    mplayerid_t all()        const;

    /// @brief Return all players' mpid but mine.
    /// @return All players' mpid excluding me
    mplayerid_t all_but_me() const;

    /// @brief Return the number of players.
    /// @return Number of players
    size_type num_players() const;

    /// @brief Clear my buffer and sync with all other players.
    void sync();

    /// @brief Send message to another player.
    /// @note Blocks until operation completes.
    /// @param to Another receiver player's pid
    /// @param message The message to be sent
    void send(playerid_t to, ByteVector &&message);

    /// @brief Send different messages to other players separately.
    /// @note Blocks until operation completes.
    /// @param to Other receiver players' mpid
    /// @param message The message to be sent
    void msend(mplayerid_t to, mByteVector&& messages);

    /// @brief Receive message from another player.
    /// @note Length of message received is determined by the sender. Blocks until operation completes.
    /// @param from Another sender player's pid
    /// @param size_hint Estimation of the size of received information
    /// @return The message to be received
    ByteVector recv(playerid_t from, size_type size_hint = 0);

    /// @brief Receive different messages from other players separately.
    /// @note Length of message received is determined by the sender. Blocks until operation completes.
    /// @param from Other sender players' mpid
    /// @param size_hint Estimation of the size of received information
    /// @return The message to be received
    mByteVector mrecv(mplayerid_t from, size_type size_hint = 0);

    /// @brief Broadcast message to all the other players.
    /// @note Blocks until operation completes.
    /// @param message The message to be broadcasted
    void broadcast(ByteVector&& message);

    /// @brief Broadcast the same message to a group of players.
    /// @note Blocks until operation completes.
    /// @param message The message to be broadcasted
    void mbroadcast(mplayerid_t to, ByteVector&& message);

    /// @brief Send message to, then receive from another player.
    /// @note The original contents of message will be destroyed. 
    ///       Length of message received is determined by the sender. Blocks until operation completes.
    /// @param pid Another player's pid
    /// @param message The message to be sent
    /// @return The message to be received
    ByteVector exchange(playerid_t pid, ByteVector &&message);

    /// @brief Send a message to the next player, and receive from the previous player.
    /// @note Length of message received is determined by the sender. Blocks until operation completes.
    /// @param offset Number of intervals between participants
    /// @param message The message to be sent
    /// @return The message to be received
    ByteVector pass_around(offset_type offset, ByteVector &&message);

    /// @brief Broadcast message, then receive from all other players.
    /// @note Length of message received is determined by the sender. Blocks until operation completes.
    /// @param message The message to be sent
    /// @return The message to be received
    mByteVector broadcast_recv(ByteVector &&message);

    /// @brief Broadcast message, then receive among a group of players. 
    /// @note Length of message received is determined by the sender. Blocks until operation completes.
    /// @param message The message to be sent
    /// @return The message to be received
    mByteVector mbroadcast_recv(mplayerid_t group, ByteVector&& message);
};

/************************ socket multi party player ************************/

namespace detail
{

/// @class SocketMultiPartyPlayer
/// @brief MultiPartyPlayer that uses socket.
template <typename SocketType>
class SocketMultiPartyPlayer : public MultiPartyPlayer
{
  public:
    using IOContext         = boost::asio::io_context;
    using WorkGuard         = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    using ThreadVector      = std::vector<std::thread>;
    using EndpointVector    = std::vector<boost::asio::ip::tcp::endpoint>;
    using CommPackageType   = CommPackage<SocketType>;
    using SocketPackageType = SocketPackage<SocketType>;

    using BitrateType       = CommPackageType::BitrateType;
    using DurationType      = CommPackageType::DurationType;

  protected:
    bool _is_running; // whether io_context is running

    IOContext       _ioc;
    WorkGuard       _work_guard;      // keep io_context alive while there is no job to do
    ThreadVector    _worker_threads;  // threads used to run io_context
    CommPackageType _comm;            // group of sockets

  protected:

    /// @brief Implementation of clearing my buffer and sync with all other players.
    void impl_sync();

    /// @brief Implementation of sending message to another player using TCP socket.
    /// @param to Another receiver player's pid
    /// @param message The message to be sent
    void        impl_send          (playerid_t to,      ByteVector &&message);

    /// @brief Implementation of receiving message from another player using TCP socket.
    /// @param from Another sender player's pid
    /// @param size_hint Estimation of the size of received information
    /// @return The message to be received
    ByteVector  impl_recv          (playerid_t from,    size_type size_hint );

    /// @brief Implementation of sending message to, then receiving from another player using TCP socket.
    /// @param pid Another player's pid
    /// @param message The message to be sent
    /// @return The message to be received
    ByteVector  impl_exchange      (playerid_t peer,    ByteVector &&message);

    /// @brief Implementation of sending a message to the next player, and receiving from the previous player using TCP socket. 
    /// @param offset Number of intervals between participants
    /// @param message The message to be sent
    /// @return The message to be received
    ByteVector  impl_pass_around   (offset_type offset, ByteVector &&message);

    /// @brief Implementation of broadcasting message, then receive from all other players using TCP socket. 
    /// @param message The message to be sent
    /// @return The message to be received
    mByteVector impl_broadcast_recv(                    ByteVector &&message);

    /// @brief Implementation of broadcasting message to all the other players using TCP socket.
    /// @param message The message to be broadcasted
    void        impl_broadcast      (                     ByteVector && message );

    /// @brief Implementation of sending different messages to other players separately using TCP socket.
    /// @param to Other receiver players' mpid
    /// @param message The message to be sent
    void        impl_msend          (mplayerid_t tos,    mByteVector && messages);

    /// @brief Implementation of receiving different messages from other players separately using TCP socket.
    /// @param from Other sender players' mpid
    /// @param size_hint Estimation of the size of received information
    /// @return The message to be received
    mByteVector impl_mrecv          (mplayerid_t froms,   size_type size_hint   );

    /// @brief Implementation of broadcasting the same message to other players using TCP socket.
    /// @param message The message to be broadcasted
    void        impl_mbroadcast     (mplayerid_t tos,     ByteVector && messages);

    /// @brief Implementation of broadcasting the same message to a group of player using TCP socket.
    /// @param group A group of player
    /// @param message The message to be broadcasted
    mByteVector impl_mbroadcast_recv(mplayerid_t group,   ByteVector && message );

    /// @brief Implementation of broadcasting message, then receiving among a group of players using TCP socket. 
    /// @param message The message to be sent
    /// @return The message to be received
    virtual SocketPackage<SocketType> get_empty_sockets() = 0;

  public:

    virtual ~SocketMultiPartyPlayer();
    SocketMultiPartyPlayer(playerid_t my_pid, size_type n_players);

    /// @brief Emulate different network conditions - set specific delay.
    /// @param tos The mpid of players who are restricted by the function
    /// @param delay Specific delay
    void set_delay (mplayerid_t tos, DurationType delay);

    /// @brief Emulate different network conditions - set specific bucket size.
    /// @param tos The mpid of players who are restricted by the function
    /// @param bitrate Specific bit rate
    /// @param capacity Specific buffer capacity
    void set_bucket(mplayerid_t tos, BitrateType bitrate, size_type capacity);

    /// @brief Judge whether the threads are running.
    /// @return If they are running, return true, otherwise return false
    bool is_running() const;

    /// @brief Run all of the players' threads.
    /// @param n_threads The number of players' thread.
    void run(size_type n_threads);

    /// @brief Stop the running threads.
    void stop();

    /// @brief Interconnect the endpoints together.
    /// @param endpoints Endpoint of the participating players
    void connect(EndpointVector const &endpoints);

    /// @brief Get network statistics.
    /// @return The network statistics such as traffic statistics
    Statistics get_statistics() const;
};

} // namespace detail

/************************ secure multi party player ************************/

/// @class SecureMultiPartyPlayer
/// @brief MultiPartyPlayer that uses SSL Socket as its socket.
class SecureMultiPartyPlayer : public detail::SocketMultiPartyPlayer<SSLSocket>
{
  protected:
    boost::asio::ssl::context _ssl_ctx;
    SocketPackage<SSLSocket> get_empty_sockets();

  public:
    static constexpr auto SSLVersion = boost::asio::ssl::context_base::tlsv12;

    using SocketType = SSLSocket;
    ~SecureMultiPartyPlayer() = default;
    SecureMultiPartyPlayer(playerid_t my_pid, size_type n_players);
  
    /// @brief Used only in SecureMultiPartyPlayer to set up SSL protocol's context.
    /// @param ssl_dir SSL protocol's verify path
    void setup_ssl_context(std::string const& ssl_dir);
};

/************************ plain multi party player ************************/

/// @class SocketMultiPartyPlayer
/// @brief MultiPartyPlayer that uses TCP Socket as its socket.
class PlainMultiPartyPlayer : public detail::SocketMultiPartyPlayer<TCPSocket>
{
  protected:
    /// @brief Get empty sockets.
    /// @return Empty socket packages
    SocketPackage<TCPSocket> get_empty_sockets();

  public:
    using SocketType = TCPSocket;
    ~PlainMultiPartyPlayer() = default;
    PlainMultiPartyPlayer(playerid_t my_pid, size_type n_players);
};

} // namespace network
