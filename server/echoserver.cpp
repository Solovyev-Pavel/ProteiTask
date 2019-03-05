#include "echoserver.h"

#include <iostream>

namespace echoserver
{

EchoServer::EchoServer(uint16_t port, uint32_t bufferSize)
    : tcpListener_(port, bufferSize)
    , udpListener_(port, bufferSize) {}

void EchoServer::run()
{
    if (!tcpListener_.isInitialized() && !udpListener_.isInitialized())
    {
        std::cerr << "Neither TCP listener nor UDP listener were initialized successfully.\n"
                     "Echo server can't be run. EXIT.\n";
        return;
    }

    // both tcp and udp listeners use same port, so it doesn't really matter which one we call getPort() from
    std::cout << ">>> Running echo server on port " << tcpListener_.getPort() << ".\n\n";

    if (tcpListener_.isInitialized())
        tcpListenerThread_.reset(new std::thread(&TcpListener::run, &tcpListener_));
    if (udpListener_.isInitialized())
        udpListenerThread_.reset(new std::thread(&UdpListener::run, &udpListener_));

    if (tcpListenerThread_)
        tcpListenerThread_->join();
    if (udpListenerThread_)
        udpListenerThread_->join();
}

}
