#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

int main(int argc, char *argv[])
{
    try {
        /// Check command line arguments.
        if (argc != 2) {
            std::cerr << "Usage: boost_as <port>\n";
            return 1;
        }

        io_service service;
        /// TODO: create server object
        /// start service
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
