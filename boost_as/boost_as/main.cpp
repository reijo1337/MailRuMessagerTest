#include "server.h"
#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

int main(int argc, char *argv[])
{
    try {
        if (argc != 2) {
            std::cerr << "Usage: boost_as <port>";
            return 1;
        }

        io_service service;
        Server server(service, atoi(argv[1]));
        service.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
