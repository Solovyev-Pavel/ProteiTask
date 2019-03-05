#ifndef INCLUDE_ONCE_F31CED1A_ED53_476B_97B6_4BD66FCF5BA0
#define INCLUDE_ONCE_F31CED1A_ED53_476B_97B6_4BD66FCF5BA0

#include "listeners.h"

#include <thread>
#include <memory>

namespace echoserver
{

/// @brief main class of the EchoServer
class EchoServer
{
public:
    /// @brief EchoServer class constructor
    /// @param port port to which the EchoServer will be listening to
    /// @param bufferSize size of the read buffers
    EchoServer(uint16_t port, uint32_t bufferSize);
    /// @brief runs the EchoServer
    void run();

private:
    TcpListener tcpListener_;                           /// < listener for TCP protocol
    UdpListener udpListener_;                           /// < listener for UDP protocol

    std::unique_ptr<std::thread> tcpListenerThread_;    /// < thread in which the TCP listener is run
    std::unique_ptr<std::thread> udpListenerThread_;    /// < thread in which the UDP listener is run
};

}

#endif // include guard
