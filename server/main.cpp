#include "echoserver.h"
#include "utils.h"
#include "globals.h"

#include <cstring>
#include <iostream>

namespace
{

constexpr auto EXPECTED_ARGUMENTS_COUNT = 2;    /// < how many arguments this applications expects in argv[]
constexpr auto PORT_ARG_INDEX = 1;              /// < index of argument, which contains port number

}

int main(int argc, char* argv[])
{
    if (argc == EXPECTED_ARGUMENTS_COUNT)
    {
        const auto port = utils::getPortFromArgumetns(argv[PORT_ARG_INDEX]);
        if (!utils::isAllowedPortNumber(port))
        {
            std::cerr << "Entered port number (" << port << ") is not in the valid range.\n"
                      << globals::acceptedPortsString;
            return globals::appExitCode;
        }

        echoserver::EchoServer server(port, globals::defaultBufferSize);
        server.run();
    }
    else
    {
        std::cout << "Usage: echoServer <port number>\n" << globals::acceptedPortsString;
        return globals::appExitCode;
    }

    return globals::appExitCode;
}
