#define FMT_HEADER_ONLY
#include "multi_party_player.hpp"

namespace network
{

/************************ basic tool ************************/

/// @brief Used in run function.
/// @param ioc IO context
/// @param n_threads Number of players' thread
/// @return Players' thread
std::vector<std::thread> run_io_context(boost::asio::io_context &ioc, std::size_t n_threads)
{
    if (ioc.stopped())
        ioc.restart();

    // construct worker threads which run io context
    std::vector<std::thread> worker_threads;
    for (std::size_t i = 0; i < n_threads; ++i) {
        worker_threads.emplace_back([&ioc]() { ioc.run(); });
    }

    return worker_threads;
}

/// @brief Used in stop function.
/// @param ioc IO context
/// @param worker_threads Players' thread
void stop_io_context(boost::asio::io_context &ioc, std::vector<std::thread> &worker_threads)
{
    // stop io context
    ioc.stop();
    // join and then destroy worker threads
    for (auto &t : worker_threads)
        t.join();
    worker_threads.clear();
}

/************************ base player ************************/

/// @brief Return my player id.
/// @return My player id.
playerid_t MultiPartyPlayer::id() const
{
    return _my_pid;
}

/// @brief Return all players' mpid.
/// @return All players' mpid including me
mplayerid_t MultiPartyPlayer::all() const
{
    return mplayerid_t::all(_n_players);
}

/// @brief Return all players' mpid but mine.
/// @return All players' mpid excluding me
mplayerid_t MultiPartyPlayer::all_but_me() const
{
    auto ans = mplayerid_t::all(_n_players);
    ans.erase(_my_pid);
    return ans;
}

/// @brief Return the number of players.
/// @return Number of players
MultiPartyPlayer::size_type MultiPartyPlayer::num_players() const
{
    return _n_players;
}

/// @brief Clear my buffer and sync with all other players.
void MultiPartyPlayer::sync()
{
    this->impl_sync();
}

/// @brief Send message to another player.
/// @note Blocks until operation completes.
/// @param to Another receiver player's pid
/// @param message The message to be sent
void MultiPartyPlayer::send(playerid_t to, ByteVector &&message_send)
{
    TimerGuard guard(_timer);
    impl_send(to, std::move(message_send));
}

/// @brief Send different messages to other players separately.
/// @note Blocks until operation completes.
/// @param to Other receiver players' mpid
/// @param message The message to be sent
void MultiPartyPlayer::msend(mplayerid_t tos, mByteVector &&messages)
{
    TimerGuard guard(_timer);
    impl_msend(tos, std::move(messages));
}

/// @brief Broadcast message to all the other players.
/// @note Blocks until operation completes.
/// @param message The message to be broadcasted
void MultiPartyPlayer::broadcast(ByteVector &&messages)
{
    TimerGuard guard(_timer);
    impl_broadcast(std::move(messages));
}

/// @brief Broadcast the same message to a group of players.
/// @note Blocks until operation completes.
/// @param message The message to be broadcasted
void MultiPartyPlayer::mbroadcast(mplayerid_t tos, ByteVector &&message)
{
    TimerGuard guard(_timer);
    impl_mbroadcast(tos, std::move(message));
}

/// @brief Receive message from another player.
/// @note Length of message received is determined by the sender. Blocks until operation completes.
/// @param from Another sender player's pid
/// @param size_hint Estimation of the size of received information
/// @return The message to be received
ByteVector MultiPartyPlayer::recv(playerid_t from, size_type size_hint)
{
    TimerGuard guard(_timer);
    return impl_recv(from, size_hint);
}

/// @brief Receive different messages from other players separately.
/// @note Length of message received is determined by the sender. Blocks until operation completes.
/// @param from Other sender players' mpid
/// @param size_hint Estimation of the size of received information
/// @return The message to be received
mByteVector MultiPartyPlayer::mrecv(mplayerid_t froms, size_type size_hint)
{
    TimerGuard guard(_timer);
    return impl_mrecv(froms, size_hint);
}

/// @brief Send message to, then receive from another player.
/// @note The original contents of message will be destroyed. 
///       Length of message received is determined by the sender. Blocks until operation completes.
/// @param pid Another player's pid
/// @param message The message to be sent
/// @return The message to be received
ByteVector MultiPartyPlayer::exchange(playerid_t peer, ByteVector &&message_send)
{
    TimerGuard guard(_timer);
    return impl_exchange(peer, std::move(message_send));
}

/// @brief Send a message to the next player, and receive from the previous player. 
/// @note Length of message received is determined by the sender. Blocks until operation completes.
/// @param offset Number of intervals between participants
/// @param message The message to be sent
/// @return The message to be received
ByteVector MultiPartyPlayer::pass_around(offset_type offset, ByteVector &&message_send)
{
    TimerGuard guard(_timer);
    return impl_pass_around(offset, std::move(message_send));
}

/// @brief Broadcast message, then receive from all other players. 
/// @note Length of message received is determined by the sender. Blocks until operation completes.
/// @param message The message to be sent
/// @return The message to be received
mByteVector MultiPartyPlayer::broadcast_recv(ByteVector &&message_send)
{
    TimerGuard guard(_timer);
    return impl_broadcast_recv(std::move(message_send));
}

/// @brief Broadcast message, then receive among a group of players. 
/// @note Length of message received is determined by the sender. Blocks until operation completes.
/// @param message The message to be sent
/// @return The message to be received
mByteVector MultiPartyPlayer::mbroadcast_recv(mplayerid_t group, ByteVector &&message)
{
    TimerGuard guard(_timer);
    return impl_mbroadcast_recv(group, std::move(message));
}

/************************ socket player ************************/

/************************ secure player ************************/

/// @brief Constructor
/// @param my_pid Player's id used to construct
/// @param n_players Number of players
SecureMultiPartyPlayer::SecureMultiPartyPlayer(playerid_t my_pid,
                                               size_type n_players)
    : SocketMultiPartyPlayer(my_pid, n_players),
      _ssl_ctx(SSLVersion)
{
    // !!!CRUCIAL!!!
    // do not create sockets before setting up ssl context completely
    // otherwise ssl handshake error will occur
}

/// @brief Used only in SecureMultiPartyPlayer to set up SSL protocol's context.
/// @param ssl_dir SSL protocol's verify path
void SecureMultiPartyPlayer::setup_ssl_context(std::string const& ssl_dir)
{
    std::string cert_file = "" + ssl_dir + "/Party" + std::to_string(this->id()) + ".crt";
    std::string key_file  = "" + ssl_dir + "/Party" + std::to_string(this->id()) + ".key";

    this->_ssl_ctx.use_certificate_file(cert_file, boost::asio::ssl::context::pem);
    this->_ssl_ctx.use_private_key_file(key_file,  boost::asio::ssl::context::pem);
    this->_ssl_ctx.add_verify_path(ssl_dir);
}

/// @brief Get empty sockets.
/// @return Empty socket packages
SocketPackage<SSLSocket> SecureMultiPartyPlayer::get_empty_sockets()
{
    return SocketPackage<SSLSocket>(_n_players, _ioc, _ssl_ctx);
}

/************************ plain player ************************/

/// @brief Constructor
/// @param my_pid Player's id used to construct
/// @param n_players Number of players
PlainMultiPartyPlayer::PlainMultiPartyPlayer(
    playerid_t my_pid,
    size_type n_players)
    : SocketMultiPartyPlayer(my_pid, n_players)
{
}

/// @brief Get empty sockets.
/// @return Empty socket packages
SocketPackage<TCPSocket> PlainMultiPartyPlayer::get_empty_sockets()
{
    return SocketPackage<TCPSocket>(_n_players, _ioc);
}

/************************ local player ************************/

} // namespace network
