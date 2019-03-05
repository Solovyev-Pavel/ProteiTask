#ifndef INCLUDE_ONCE_B568A299_C004_47E8_809B_CB7D02EC3BB3
#define INCLUDE_ONCE_B568A299_C004_47E8_809B_CB7D02EC3BB3

#include "globals.h"

#include <regex>
#include <string>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>

namespace utils
{

/// @brief changes all letters in text to lowercase
/// @param text text to be changed into all-lowercase
void textToLower(char *text)
{
    const auto textLength = strlen(text);
    for (std::size_t i = 0; i < textLength; ++i)
        text[i] = tolower(text[i]);
}

/// @brief checks whether an application is allowed to try to use this port number
/// @param portNumber port number given to application via arguments
/// @returns true is portNumber is in allowed range of port numbers, false - otherwise
bool isAllowedPortNumber(int portNumber)
{
    return portNumber >= globals::minPortNumber && portNumber <= globals::maxPortNumber;
}

/// @brief tries to get server's port from argument
/// @param portArg characters of argument that is supposed to hold port number value
/// @returns port number or 0 if unable to read port number from argument
int getPortFromArgumetns(const char* portNumerArg)
{
    std::string str;
    str.append(portNumerArg, portNumerArg + strlen(portNumerArg));
    return std::stoi(str);
}

/// @brief tries to read protocol type from string
/// @param protocolTypeString string that (probably) contains protocol type
/// @returns 0 if TCP, 1 if UDP, -1 is unable to determine
int getClientProtocolType(const char* protocolTypeString)
{
    if (strcmp(protocolTypeString, globals::tcpText.c_str()) == 0)
        return 0;   // ClientProtocol::TCP
    else if (strcmp(protocolTypeString, globals::udpText.c_str()) == 0)
        return 1;   // ClientProtocol::UDP

    return -1;      // ClientProtocol::Unrecognized
}

/// @brief checks whether user-entered ip v4 address is valid
/// @param ipString user-entered string that (probably) contains ip v4 address
/// @returns true if string contains valid ip v4 address, false - otherwise
bool ipIsValid(const char* ipString)
{
    sockaddr_in temp;
    return inet_pton(AF_INET, ipString, &temp.sin_addr) != globals::failureToParseIpCode;
}

}

#endif // include guard
