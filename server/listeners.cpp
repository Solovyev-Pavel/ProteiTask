#include "listeners.h"
#include "globals.h"

#include <regex>
#include <thread>
#include <cstring>
#include <numeric>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace echoserver
{

//---------------------------------------------------------

BaseListener::BaseListener(uint16_t port, uint32_t bufferSize)
    : bufferSize_(bufferSize)
{
    std::memset(&socketAddress_, 0x00, sizeof socketAddress_);
    socketAddress_.sin_family = AF_INET;
    socketAddress_.sin_addr.s_addr = htonl(INADDR_ANY);
    socketAddress_.sin_port = htons(port);
}

BaseListener::~BaseListener()
{
    close(socketDescriptor_);
}

//---------------------------------------------------------

bool BaseListener::prepareSocket(int type, int protocol, const std::string &typeString)
{
    const auto socketSize = sizeof socketAddress_;
    socketDescriptor_ = socket(PF_INET, type, protocol);
    if (socketDescriptor_ == globals::failureToInitCode)
    {
        std::cerr << "ERROR: failed to create a " << typeString << " socket!\n";
        return false;
    }

    if (bind(socketDescriptor_, reinterpret_cast<sockaddr*>(&socketAddress_), socketSize) == globals::failureToBindCode)
    {
        std::cerr << "ERROR: failed to bind a " << typeString << " socket to port "
                  << ntohs(socketAddress_.sin_port) << "!\n";
        close(socketDescriptor_);
        return false;
    }

    return true;
}

//---------------------------------------------------------

std::vector<int> extractNumbers(const std::string &message)
{
    std::vector<int> numbers;
    const std::regex numberRegex("-?\\d+");
    const auto numBegin = std::sregex_iterator(message.begin(), message.end(), numberRegex);
    const auto numEnd = std::sregex_iterator();

    for (std::sregex_iterator i = numBegin; i != numEnd; ++i)
        numbers.emplace_back( std::stoi((*i).str()) );

    return numbers;
}

void BaseListener::processMessage(const std::string &message)
{
    auto numbers = extractNumbers(message);

    if (!numbers.empty())
    {
        std::sort(numbers.begin(), numbers.end(), [](int lhs, int rhs){ return rhs < lhs; });

        const std::string numberList = std::accumulate(std::next(numbers.cbegin()), numbers.cend(),
                                                       std::to_string(numbers.front()),
                                                       [](std::string a, int b)
                                                       {
                                                           return std::move(a) + " " + std::to_string(b);
                                                       });

        std::cout << "Numbers within message: " << numberList <<  "\n";
        std::cout << "Min number: " << numbers.back() << "; max number: " << numbers.front() << "\n";
        std::cout << "Sum of numbers: " << std::accumulate(numbers.cbegin(), numbers.cend(), 0) << "\n";
    }

    std::cout << "\n";
}

//=========================================================

TcpListener::TcpListener(uint16_t port, uint32_t bufferSize)
    : BaseListener(port, bufferSize)
{
    isInitialized_ = prepareSocket(SOCK_STREAM, IPPROTO_TCP, "TCP");
}

//---------------------------------------------------------

void TcpListener::handleConnection(int connectionSocket, sockaddr_in clientAddress)
{
    char *readBuffer = new char[bufferSize_];
    while (true)
    {
        const auto rSize = recv(connectionSocket, readBuffer, bufferSize_, 0);
        if (rSize < 0)
        {
            std::cerr << "ERROR while receiving message from " << inet_ntoa(clientAddress.sin_addr)
                    << ":" << ntohs(clientAddress.sin_port) << "...\n";
            close(connectionSocket);
            break;
        }
        else if (rSize == globals::disconnectionMsgLength)
        {
            // TCP connection was closed
            close(connectionSocket);
            break;
        }

        std::string messageString;
        messageString.append(readBuffer, readBuffer + rSize);
        std::memset(readBuffer, 0x00, bufferSize_);

        // printing message
        std::cout << "Message from " << inet_ntoa(clientAddress.sin_addr) << ":"
                  << ntohs(clientAddress.sin_port) << ": " << messageString << "\n";

        // sending echo
        send(connectionSocket, messageString.c_str(), rSize, 0);

        processMessage(messageString);
    }
    delete readBuffer;
}


void TcpListener::run()
{
    if (!isInitialized_)
    {
        std::cerr << globals::listenerUninitSocketError;
        return;
    }

    int connection;
    sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof clientAddress;

    if (listen(socketDescriptor_, 10) == globals::failureToListenCode)
    {
        std::cerr << "ERROR: failed to listen to socket...\n";
        return;
    }

    while (true)
    {
        connection = accept(socketDescriptor_, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressLength);
        if (connection < 0)
        {
            std::cerr << "ERROR: failed to accept connection from " << inet_ntoa(clientAddress.sin_addr)
                      << ":" << ntohs(clientAddress.sin_port) << "...\n";
            continue;
        }

        std::thread(&TcpListener::handleConnection, this, connection, clientAddress).detach();
    }
}

//=========================================================

UdpListener::UdpListener(uint16_t port, uint32_t bufferSize)
    : BaseListener(port, bufferSize)
{
    isInitialized_ = prepareSocket(SOCK_DGRAM, IPPROTO_UDP, "UDP");
}

//---------------------------------------------------------

void UdpListener::run()
{
    if (!isInitialized_)
    {
        std::cerr << globals::listenerUninitSocketError;
        return;
    }

    char *readBuffer = new char [bufferSize_];
    sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof clientAddress;

    while (true)
    {
        const auto rSize = recvfrom(socketDescriptor_, readBuffer, bufferSize_,
                                    MSG_WAITALL, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressLength);

        if (rSize < 0)
        {
            std::cerr << "ERROR while receiving message from " << inet_ntoa(clientAddress.sin_addr)
                      << ":" << ntohs(clientAddress.sin_port) << "...\n";
            continue;
        }

        std::string messageString;
        messageString.append(readBuffer, readBuffer + rSize);
        std::memset(readBuffer, 0x00, bufferSize_);

        // printing message
        std::cout << "Message from " << inet_ntoa(clientAddress.sin_addr) << ":"
                  << ntohs(clientAddress.sin_port) << ": " << messageString << "\n";

        // sending echo
        sendto(socketDescriptor_, messageString.c_str(), rSize,
               MSG_CONFIRM, reinterpret_cast<sockaddr*>(&clientAddress), clientAddressLength);

        processMessage(messageString);
    }

    delete readBuffer;
}

}
