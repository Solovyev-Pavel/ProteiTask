#ifndef INCLUDE_ONCE_49892D43_0CB3_4988_B2DC_861E13762096
#define INCLUDE_ONCE_49892D43_0CB3_4988_B2DC_861E13762096

#include <string>
#include <netinet/in.h>

namespace echoserver
{

/// @brief base class for echoServer listeners
class BaseListener
{
public:
    /// @brief BaseListener class constructor
    /// @param port port the listener will be listening to if its socket is created and bound successfully
    /// @param bufferSize size of the read buffer
    BaseListener(uint16_t port, uint32_t bufferSize);
    /// @brief BaseListener class destructor
    ~BaseListener();

    /// @brief tells, whether the listener object was initialized successfully
    /// @returns true if the listener was initialized successfully, false - otherwise
    bool isInitialized() const { return isInitialized_; }
    /// @brief runs the listener
    virtual void run() = 0;
    /// @brief tells, which port is bound to the listener's socket
    /// @return port number
    int getPort() const { return ntohs(socketAddress_.sin_port); }

protected:
    /// @brief creates and binds a socket
    /// @param type specifies type of the socket to be created
    /// @param protocol specifies protocol that will be used the socket
    /// @param typeString socket's type string, needed for error messages
    /// @returns true if socket was created and binded successfully, false - otherwise
    bool prepareSocket(int type, int protocol, const std::string &typeString);
    /// @brief processes message received by the listener's socket
    /// @param message text of the message
    void processMessage(const std::string &message);

    int socketDescriptor_;          /// < descriptor of the listener's socket
    sockaddr_in socketAddress_;     /// < address bound to the listener's socket
    uint32_t bufferSize_;           /// < size of the listener's read buffer

    bool isInitialized_ = false;    /// < true if the listener's was socket created and bound successfully
};

/// @brief class for echoServer listener that uses TCP protocol
class TcpListener : public BaseListener
{
public:
    /// @brief TcpListener class constructor
    /// @param port port the listener will be listening to if its socket is created and bound successfully
    /// @param bufferSize size of the read buffer
    TcpListener(uint16_t port, uint32_t bufferSize);
    /// @brief runs the TCP listener
    void run() override;
private:
    /// @brief handles connection with a TCP client
    /// @param connectionSocket descriptor of client's socket
    /// @param clientAddress client's address data
    void handleConnection(int connectionSocket, sockaddr_in clientAddress);
};

/// @brief class for echoServer listener that uses UDP protocol
class UdpListener : public BaseListener
{
public:
    /// @brief UdpListener class constructor
    /// @param port port the listener will be listening to if its socket is created and bound successfully
    /// @param bufferSize size of the read buffer
    UdpListener(uint16_t port, uint32_t bufferSize);
    /// @brief runs the UDP listener
    void run() override;
};

}

#endif
