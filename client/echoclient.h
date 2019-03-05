#ifndef INCLUDE_ONCE_20B79509_8CB6_41DE_A433_A86A2B72C365
#define INCLUDE_ONCE_20B79509_8CB6_41DE_A433_A86A2B72C365

#include <string>
#include <netinet/in.h>

namespace echoclient
{

/// @brief client protocol types enum
enum class ClientProtocol
{
    Unrecognized = -1,
    TCP,
    UDP,
};

/// @brief base class for echoClient senders
class BaseSender
{
public:
    /// @brief BaseSender class constructor
    /// @param serverIp string containing ip v4 address of echoServer with which echoClient will communicate
    /// @param serverPort port number of echoServer with which echoClient will communicate
    /// @param bufferSize size of the write buffer
    BaseSender(const std::string &serverIp, uint16_t serverPort, uint32_t bufferSize);
    /// @brief BaseSender class destructor
    ~BaseSender();

    /// @brief tells, whether the listener object was initialized successfully
    /// @returns true if the listener was initialized successfully, false - otherwise
    bool isInitialized() const { return isInitialized_; }
    /// @brief runs echoClient sender
    virtual void run() = 0;

protected:
    /// @brief creates sender's socket
    /// @param specifies type of the socket to be created
    /// @param typeString socket's type string, needed for error messages
    /// @returns true if socket was created and binded successfully, false - otherwise
    bool prepareSocket(int type, const std::string &typeString);

    int socketDescriptor_;          /// < descriptor of the listener's socket
    sockaddr_in serverAddress_;     /// < address bound to the listener's socket
    uint32_t bufferSize_;           /// < size of the listener's read buffer

    bool isInitialized_ = false;    /// < true if the listener's was socket created and bound successfully
};

/// @brief class for echoClient sender that uses TCP protocol
class TcpSender : public BaseSender
{
public:
    /// @brief TcpSender class constructor
    /// @param serverIp string containing ip v4 address of echoServer with which echoClient will communicate
    /// @param serverPort port number of echoServer with which echoClient will communicate
    /// @param bufferSize size of the write buffer
    TcpSender(const std::string &serverIp, uint16_t serverPort, uint32_t bufferSize);
    /// @brief runs the echoClient TCP sender
    void run() override;
};

/// @brief class for echoClient sender that uses UDP protocol
class UdpSender : public BaseSender
{
public:
    /// @brief UdpSender class constructor
    /// @param serverIp string containing ip v4 address of echoServer with which echoClient will communicate
    /// @param serverPort port number of echoServer with which echoClient will communicate
    /// @param bufferSize size of the write buffer
    UdpSender(const std::string &serverIp, uint16_t serverPort, uint32_t bufferSize);
    /// @brief runs the echoClient UDP sender
    void run() override;
private:
    /// @brief method that handles actually sending message to EchoServer and getting its response
    /// @param message string with text to send
    void sendMessage(const std::string message);
};

}

#endif // include guard
