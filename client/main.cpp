#include "utils.h"
#include "globals.h"
#include "echoclient.h"

#include <memory>
#include <cstring>
#include <iostream>

namespace
{

constexpr auto EXPECTED_ARGUMENTS_COUNT = 4;    /// < how many arguments this applications expects in argv[]
constexpr auto PROTOCOL_ARG_INDEX = 1;          /// < index of argument, which contains protocol string
constexpr auto IPADDRESS_ARG_INDEX = 2;         /// < index of argument, which contains echoServer's ip address
constexpr auto PORT_ARG_INDEX = 3;              /// < index of argument, which contains echoServer's port number

/// @brief print usage hint for application
void printUsageHint()
{
    std::cout << "Usage: echoServer tcp|udp <server ip> <server port>\n" << globals::acceptedPortsString;
}

}

int main(int argc, char* argv[])
{
    if (argc == EXPECTED_ARGUMENTS_COUNT)
    {
        // argv[1] = client mode :: UDP or TCP
        utils::textToLower(argv[PROTOCOL_ARG_INDEX]);
        const auto protocol = static_cast<echoclient::ClientProtocol>(utils::getClientProtocolType(argv[PROTOCOL_ARG_INDEX]));
        if (protocol == echoclient::ClientProtocol::Unrecognized)
        {
            std::cerr << "Unrecognized protocol type. Allowed protocol types are '" << globals::tcpText
                      << "' and '" << globals::udpText << "'.\n";
            printUsageHint();
            return globals::appExitCode;
        }

        // argv[2] = server's ip
        std::string ipString;
        if (!utils::ipIsValid(argv[IPADDRESS_ARG_INDEX]))
        {
            std::cerr << "Entered ip v4 address '" << argv[IPADDRESS_ARG_INDEX] << "' is not valid.\n";
            printUsageHint();
            return globals::appExitCode;
        }
        ipString.append(argv[IPADDRESS_ARG_INDEX], argv[IPADDRESS_ARG_INDEX] + strlen(argv[IPADDRESS_ARG_INDEX]));

        // argv[3] = server's port number
        const auto serverPort = utils::getPortFromArgumetns(argv[PORT_ARG_INDEX]);
        if (!utils::isAllowedPortNumber(serverPort))
        {
            std::cerr << "Entered port number (" << serverPort << ") is not in the valid range.\n";
            printUsageHint();
            return globals::appExitCode;
        }

        std::unique_ptr<echoclient::BaseSender> sender;
        switch (protocol)
        {
        case echoclient::ClientProtocol::TCP:
            sender.reset(new echoclient::TcpSender(ipString, serverPort, globals::defaultBufferSize));
            break;
        case echoclient::ClientProtocol::UDP:
            sender.reset(new echoclient::UdpSender(ipString, serverPort, globals::defaultBufferSize));
            break;
        default:
            break;
        }

        if (sender && sender->isInitialized())
            sender->run();
    }
    else
    {
        printUsageHint();
        return globals::appExitCode;
    }

    return globals::appExitCode;
}
