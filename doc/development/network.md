# Network

通信模块主要通过以下两个虚基类提供通信接口。消息的内容存储在 ByteVector 中。

```c++
// #include "network/network.hpp"

class TwoPartyPlayer {
    void send(ByteVector&&);
    ByteVector recv();
    ByteVector exchange(ByteVector&&);
};

class MultiPartyPlayer {
    void        send          (playerid_t, ByteVector&&);
    ByteVector  recv          (playerid_t);
    ByteVector  exchange      (playerid_t, ByteVector&&);
    ByteVector  pass_around   (offset_t,   ByteVector&&);
    mByteVector broadcast_recv(ByteVector&&);
};
```

具体功能实现放在以下几个类中。其中，`Secure` 代表使用 SSL 加密通信；`Plain` 表示使用 TCP 明文通信。

* Secure[Two|Multi]PartyPlayer
* Plain[Two|Multi]PartyPlayer

## 通信操作定义

假设有 N 个参与方，每个参与方都有自己的 ID, 分别为 {0, 1, ..., N-1}。

1. `send`：发送一条消息给对方。
2. `recv`: 接收一条对方发送过来的消息。
3. `exchange`: 发送一条消息给对方，并接收一条对方发送的消息。
4. `pass_around`: 发送一条消息给下游参与方 (my_pid+offset)，并接收一条上游参与方(my_pid-offset) 发送的消息。
5. `broadcast_recv`: 将一条消息广播给所有人，并返回从其他参与方接收到的消息。返回值是一个长度为 N 的数组，其中数组的第 `i` 个位置存放着 $P_i$ 发送过来的消息。数组第 `my_pid` 个位置是一条空消息。

## 延迟和带宽设置

设置从本地发送到其他参与方的延迟和带宽。带宽控制通过令牌桶算法实现。

* set_delay(mplayerid_t, std::chrono::nanoseconds)：设置通信延迟，该延迟会叠加在网络的固有延迟上，因此总延迟=固有延迟+新增延迟。
* set_bucket(mplayerid_t, GigaBitsPerSecond, std::size_t)：设置令牌桶的令牌生成速率以及令牌桶的容量。

## 使用加密通信

1. 安装 openssl。
2. 生成 SSL 证书和密钥。

```bash
i=${MY_PLAYER_ID}
ssl_dir=${PATH_TO_KEYS_AND_CERTS}

openssl req -newkey rsa -nodes -x509 -out $ssl_dir/Party$i.crt -keyout $ssl_dir/Party$i.key -subj "/CN=Party$i"

c_rehash $ssl_dir
```

在使用 Secure[Two|Multi]PartyPlayer 建立连接之前，通过 `setup_ssl_context(ssl_dir)` 设置证书和密钥的路径。

## 注意事项

* 各个参与方的通信模块必须按照参与方的编号从小到大的顺序依次启动。
* 在建立连接以及收发消息之前，要先调用 `run()` 方法以启动任务处理线程。
* Player 类不可复制，不可移动。

## Example

```C++
#include "network/network.hpp"

#include "serialization/serialization.hpp"

#include <chrono>
#include <cstdlib>
#include <cassert>

void test_network(int argc, char* argv[])
{
    assert(argc == 3);
    auto my_id = atoi(argv[1]);         // 本参与方的编号
    auto n_players = atoi(argv[2]);     // 总参与方数量
    auto n_threads = 1;                 // 网络任务处理线程的数量
    std::string ssl_dir = "~/conf/";    // SSL 证书和密钥的存放路径

    // 创建通信对象
    network::SecureMultiPartyPlayer player(my_id, n_players);

    // 设置 SSL 证书和密钥的存放路径
    player.setup_ssl_context(ssl_dir);

    // 创建任务处理线程并开始运行
    player.run(n_threads);
    // 设置网络连接的 IP 和端口号
    using address = boost::asio::ip::address;
    using endpoint = boost::asio::ip::tcp::endpoint;
    auto ep0 = endpoint{ address::from_string("127.0.0.1"), 44444 };  // P0 的地址和端口
    auto ep1 = endpoint{ address::from_string("127.0.0.1"), 55555 };  // P1 的地址和端口
    player.connect({ep0, ep1});

    // 设置延迟和带宽
    auto delay = std::chrono::nanoseconds(1000000 * 5); // 消息发送延迟
    auto bitrate = network::GigaBitsPerSecond(1.0);     // 令牌生成速率
    auto capacity = 1000000000;                         // 令牌桶容量（字节）
    player.set_delay(player.all(), delay);
    player.set_bucket(player.all(), bitrate, capacity);

    network::MultiPartyPlayer* mp = &player;

    // 设置发送消息的内容
    Serializer sr;
    std::string message = "hello from player " + std::to_string(my_id);
    sr << message;

    // 发送并接受一条消息
    auto msg_recv = mp->exchange(1-my_id, sr.finalize());

    // 解析接收的消息
    std::string msg;
    Deserializer dr(std::move(msg_recv));
    dr >> msg;
    std::cout<<msg<<std::endl;
}
```
