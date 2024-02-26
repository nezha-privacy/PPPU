# **Network**
* Contains a **MultiPartyPlayer** object that implements communication among n party players. Also provides a **TwoPartyPlayer** object used between 2 party players.
* Both MultiPartyPlayer and TwoPartyPlayer have Secure and Plain version, which used SSL and TCP as its sockets respectively, which are contains real APIs used in computation, in **multi_party_player.h** and **two_party_player.h** respectively.
* Connection functions are in **mp_connect.h**, the sockets are in **socket_package.h**, player id are in **playerid.h** and communication information are in **statistics.h**. Other headers contains some implementations.
  ***
  ***
  ### **./multi_party_player.h**
  ***
  #### **class network::MultiPartyPlayer**
  Player entity under multi party communication conditions. It is a base class with basic network interface.
  ***
  #### **MultiPartyPlayer(my_pid, n_players)**
  Constructor.
  ##### **Parameters**
  * my_pid - Player's id used to construct
  * n_players - Number of players
  ***
  #### **MultiPartyPlayer.id()**
  Return my player id.
  ##### **Returns**
  * My player id
  ***
  #### **MultiPartyPlayer.all()**
  Return all players' mpid.
  ##### **Returns**
  * All players' mpid including me
  ***
  #### **MultiPartyPlayer.all_but_me()**
  Return all players' mpid but mine.
  ##### **Returns**
  * All players' mpid excluding me
  ***
  #### **MultiPartyPlayer.num_players()**
  Return the number of players.
  ##### **Returns**
  * Number of players
  ***
  #### **sync()**
  Clear my buffer and sync with all other players.
  ***
  #### **MultiPartyPlayer.send(to, message_send)**
  Send message to another player.
  ##### **Parameters**
  * to - Another receiver player's pid
  * message - The message to be sent
  ***
  #### **MultiPartyPlayer.msend(tos, messages)**
  Send different messages to other players separately.
  ##### **Parameters**
  * to - Other receiver players' mpid
  * message - The message to be sent
  ***
  #### **MultiPartyPlayer.broadcast(messages)**
  Broadcast message to all the other players.
  ##### **Parameters**
  * messages - The messages to be broadcasted
  ***
  #### **MultiPartyPlayer.mbroadcast(tos, message)**
  Broadcast the same message to a group of players.
  ##### **Parameters**
  * message - The message to be broadcasted
  ***
  #### **MultiPartyPlayer.recv(from, size_hint)**
  Receive message from another player.
  ##### **Parameters**
  * from - Another sender player's pid
  * size_hint - Estimation of the size of received information
  ##### **Returns**
  * The message to be received
  ***
  #### **MultiPartyPlayer.mrecv(froms, size_hint)**
  Receive different messages from other players separately.
  ##### **Parameters**
  * from - Other sender players' mpid
  * size_hint - Estimation of the size of received information
  ##### **Returns**
  * The messages to be received
  ***
  #### **MultiPartyPlayer.exchange(peer, message_send)**
  Send message to, then receive from another player.
  ##### **Parameters**
  * pid - Another player's pid
  * message_send - The message to be sent
  ##### **Returns**
  * The message to be received
  ***
  #### **MultiPartyPlayer.pass_around(offset, message_send)**
  Send a message to the next player, and receive from the previous player.
  ##### **Parameters**
  * offset - Number of intervals between participants
  * message_send - The message to be sent
  ##### **Returns**
  * The message to be received
  ***
  #### **MultiPartyPlayer.broadcast_recv(message_send)**
  Broadcast message, then receive from all other players. 
  ##### **Parameters**
  * message_send - The message to be sent
  ##### **Returns**
  * The messages to be received
  ***
  #### **MultiPartyPlayer.mbroadcast_recv(group, message)**
  Broadcast message, then receive among a group of players.
  ##### **Parameters**
  * group - A group of players
  * message The message to be sent
  ##### **Returns**
  * The messages to be received
  ***
  #### **SecureMultiPartyPlayer(my_pid, n_players)**
  Constructor.
  ##### **Parameters**
  * my_pid - Player's id used to construct
  * n_players - Number of players
  ***
  #### **SecureMultiPartyPlayer.setup_ssl_context(ssl_dir)**
  Used only in SecureMultiPartyPlayer to set up SSL protocol's context.
  ##### **Parameters**
  * ssl_dir - SSL protocol's verify path
  ***
  #### **SecureMultiPartyPlayer.get_empty_sockets()**
  Get empty sockets.
  ##### **Returns**
  * Empty socket packages
  ***
  #### **SecureMultiPartyPlayer.set_delay(mplayerid_t tos, DurationType delay)**
  Emulate different network conditions - set specific delay.
  ##### **Parameters**
  * tos - The mpid of players who are restricted by the function
  * delay - Specific delay
  ***
  #### **SecureMultiPartyPlayer.set_bucket(tos, rate, capacity)**
  Emulate different network conditions - set specific bucket size.
  
  arameters**
  * tos - The mpid of players who are restricted by the function
  * bitrate - Specific bit rate
  * capacity - Specific buffer capacity
  ***
  #### **SecureMultiPartyPlayer.get_statistics()**
  Get network statistics.
  ##### **Returns**
  * The network statistics such as traffic statistics
  ***
  #### **SecureMultiPartyPlayer.is_running()**
  Judge whether the threads are running.
  ##### **Returns**
  * If they are running, return true, otherwise return false
  ***
  #### **SecureMultiPartyPlayer.run(size_type n_threads)**
  Run all of the players' threads.
  ##### **Parameters**
  * n_threads The number of players' thread.
  ***
  #### **SecureMultiPartyPlayer.stop()**
  Stop the running threads.
  ***
  #### **SecureMultiPartyPlayer.connect(endpoints)**
  Interconnect the endpoints together.
  ##### **Parameters**
  * endpoints - Endpoint of the participating players
  ***
  #### **class SocketMultiPartyPlayer**
  MultiPartyPlayer that uses TCP Socket as its socket.
  ***
  #### **class SecureMultiPartyPlayer**
  MultiPartyPlayer that uses SSL Socket as its socket.
  ***
  ***
  ### **./playerid.h**
  ***
  #### **class mplayerid_t**
  Be similar with std::set<playerid_t>.
  ***
  #### **mplayerid_t.all(n_players)**
  Get all the players' mpid.
  ##### **Parameters**
  * n_players - The number of players
  ***
  #### **mplayerid_t.all_but(n_players, but)**
  Get all the players' mpid except someone.
  ##### **Parameters**
  * n_players - The number of players
  * but - Without someone
  ***
  #### **mplayerid_t.empty()**
  Determine whether the set is empty.
  ##### **Returns**
  * If it is empty, return true
  ***
  #### **mplayerid_t.size()**
  Get the size of the set.
  ##### **Returns**
  * The size of the set
  ***
  #### **mplayerid_t.max_size()**
  Get the size of the set.
  ##### **Returns**
  * The size of the set
  ***
  #### **clear()**
  Clear the pid set of this object.
  ***
  #### **mplayerid_t.insert(pid)**
  Insert a new pid into the set from input.
  ##### **Parameters**
  * playerid_t - The new pid from input
  ***
  #### **mplayerid_t.erase(pid)**
  Erase a specific pid from the set.
  ##### **Parameters**
  * playerid_t - The pid to be erased
  ***
  #### **mplayerid_t.swap(other)**
  Swap two sets of this object and the input object.
  ##### **Parameters**
  * mplayerid_t - The input mplayer pids object
  ***
  #### **mplayerid_t.contains(pid)**
  Determine whether this object's set contains specific pid from input.
  ##### **Parameters**
  * playerid_t - The pid to be determined
  ##### **Returns**
  * Whether the input pid is in this object's set
  ***
  #### **mplayerid_t.merge(other)**
  Merge two sets of this object and the input object, the result stored in this object ( set union ).
  ##### **Parameters**
  * mplayerid_t - The input mplayer pids object
  ***
  #### **class iterator**
  Range based for.
  ***
  #### **iterator.begin()**
  The begin of iterator of mplayerid.
  ***
  #### **iterator.end()**
  The end of iterator of mplayerid.
  ***
  ***
  ### **./statistics.h**
  ***
  #### **struct Statistics**
  Record the number of bytes sent and received in this communication.
  ***
  ***