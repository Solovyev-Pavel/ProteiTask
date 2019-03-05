#include "echoclient.h"
#include "globals.h"

#include <future>
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace echoclient
{

//---------------------------------------------------------

BaseSender::BaseSender(const std::string &serverIp, uint16_t serverPort, uint32_t bufferSize)
    : bufferSize_(bufferSize)
{
    std::memset(&serverAddress_, 0x00, sizeof serverAddress_);
    serverAddress_.sin_family = AF_INET;
    serverAddress_.sin_addr.s_addr = inet_addr(serverIp.c_str());
    serverAddress_.sin_port = htons(serverPort);
}

BaseSender::~BaseSender()
{
    close(socketDescriptor_);
}

//---------------------------------------------------------

bool BaseSender::prepareSocket(int type, const std::string &typeString)
{
    socketDescriptor_ = socket(AF_INET, type, 0);
    if (socketDescriptor_ == globals::failureToInitCode)
    {
        std::cerr << "ERROR: failed to create a " << typeString << " socket!\n";
        return false;
    }

    return true;
}

//=========================================================

TcpSender::TcpSender(const std::string &serverIp, uint16_t serverPort, uint32_t bufferSize)
    : BaseSender(serverIp, serverPort, bufferSize)
{
    isInitialized_ = prepareSocket(SOCK_STREAM, "TCP");
}

//---------------------------------------------------------

void TcpSender::run()
{
    if (!isInitialized_)
    {
        std::cerr << globals::senderUninitSocketError;
        return;
    }

    const socklen_t serverAddressLength = sizeof serverAddress_;
    const auto connection = connect(socketDescriptor_, reinterpret_cast<sockaddr*>(&serverAddress_), serverAddressLength);
    if (connection == globals::failureToConnectCode)
    {
        std::cerr << "ERROR: failed to connect to the EchoServer@" << inet_ntoa(serverAddress_.sin_addr)
                  << ":" << ntohs(serverAddress_.sin_port) << ".\n";
        return;
    }

    std::cout << globals::echoClientRunMessage;
    char *buffer = new char [bufferSize_];
    std::string inputString;

    while (true)
    {
        std::getline(std::cin, inputString);
        if (inputString == "q" || inputString == "quit")
            break;

        const auto strLenToSend = std::min(static_cast<uint32_t>(inputString.size()), bufferSize_);
        std::memset(buffer, 0x00, bufferSize_);
        std::memcpy(buffer, inputString.c_str(), strLenToSend);

        const auto sSize = send(socketDescriptor_, buffer, strLenToSend, 0);
        if (sSize < 0)
        {
            std::cerr << "Failed to send message to EchoServer@" << inet_ntoa(serverAddress_.sin_addr)
                      << ":" << ntohs(serverAddress_.sin_port) << ".\n";
        }

        std::memset(buffer, 0x00, bufferSize_);
        const auto rSize = recv(socketDescriptor_, buffer, bufferSize_, 0);
        if (rSize < 0)
        {
            std::cerr << "Failed to receive a response from EchoServer@" << inet_ntoa(serverAddress_.sin_addr)
                      << ":" << ntohs(serverAddress_.sin_port) << ".\n";
        }

        std::cout << "EchoServer@" << inet_ntoa(serverAddress_.sin_addr) << ":" << ntohs(serverAddress_.sin_port)
                  << " response: " << buffer << "\n";
    }
    delete buffer;
}

//=========================================================

UdpSender::UdpSender(const std::string &serverIp, uint16_t serverPort, uint32_t bufferSize)
    : BaseSender(serverIp, serverPort, bufferSize)
{
    isInitialized_ = prepareSocket(SOCK_DGRAM, "UDP");
}

//---------------------------------------------------------

void UdpSender::sendMessage(const std::string message)
{
    socklen_t serverAddressLength = sizeof serverAddress_;
    const auto strLenToSend = std::min(static_cast<uint32_t>(message.size()), bufferSize_);

    sendto(socketDescriptor_, message.c_str(), strLenToSend, MSG_CONFIRM,
                              reinterpret_cast<sockaddr*>(&serverAddress_), serverAddressLength);

    char *buffer = new char [bufferSize_];
    std::memset(buffer, 0x00, bufferSize_);
    const auto rSize = recvfrom(socketDescriptor_, buffer, bufferSize_, MSG_WAITALL,
                                reinterpret_cast<sockaddr*>(&serverAddress_), &serverAddressLength);
    if (rSize < 0)
    {
        std::cerr << "Failed to receive a response from EchoServer@" << inet_ntoa(serverAddress_.sin_addr)
                  << ":" << ntohs(serverAddress_.sin_port) << ".\n";
    }
    else
    {
        std::cout << "EchoServer@" << inet_ntoa(serverAddress_.sin_addr) << ":" << ntohs(serverAddress_.sin_port)
                  << " response: " << buffer << "\n";
    }
    delete buffer;
}

void UdpSender::run()
{
    if (!isInitialized_)
    {
        std::cerr << globals::senderUninitSocketError;
        return;
    }

    std::cout << globals::echoClientRunMessage;
    std::string inputString;

    while (true)
    {
        std::getline(std::cin, inputString);
        if (inputString == "q" || inputString == "quit")
            break;

        std::thread(&UdpSender::sendMessage, this, inputString).detach();
    }
}

}
