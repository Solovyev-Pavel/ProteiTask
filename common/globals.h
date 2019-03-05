#ifndef INCLUDE_ONCE_B5585C58_AC8E_474A_A22C_F750EB0A7CEB
#define INCLUDE_ONCE_B5585C58_AC8E_474A_A22C_F750EB0A7CEB

#include <string>
#include <cstring>

namespace globals
{

constexpr auto appExitCode = 0;                 /// < application exit code
constexpr auto defaultBufferSize = 64 * 1024;   /// < default size for read / write buffers

//constexpr auto minPortNumber = 0;             // do not allow usage of well-known ports...
constexpr auto minPortNumber = 1024;            /// < lowest port number applications are allowed to use
constexpr auto maxPortNumber = 65535;           /// < highest port number applications are allowed to use

const std::string acceptedPortsString = "Accepted port numbers: " + std::to_string(minPortNumber) +
                                        " - " + std::to_string(maxPortNumber) + "\n";
const std::string senderUninitSocketError = "ERROR: unable to run client with a socket that wasn't initialized properly.\n";
const std::string listenerUninitSocketError = "ERROR: unable to run listener that wasn't initialized properly.\n";
const std::string echoClientRunMessage = "Please type message to send to the echo server, or 'q' / 'quit' to exit:\n";

const std::string tcpText = "tcp";              /// < tcp string
const std::string udpText = "udp";              /// < udp string

constexpr auto failureToInitCode = -1;          /// < failure to initialize socket error code
constexpr auto failureToBindCode = -1;          /// < failure to bind socket error code
constexpr auto failureToListenCode = -1;        /// < socket failed to listen error code
constexpr auto failureToConnectCode = -1;       /// < socket failed to connect error code
constexpr auto failureToParseIpCode = -1;       /// < return value of inet_pton method when it can't parse ip from text
constexpr auto disconnectionMsgLength = 0;      /// < length of message that signals client disconnection

}

#endif // include guard
