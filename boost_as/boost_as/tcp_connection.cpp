#include "tcp_connection.h"

#include <boost/bind.hpp>
#include <iostream>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

TcpConnection::TcpConnection(std::shared_ptr<tcp::socket> &&socket)
    : m_sock(socket)
{
}

void TcpConnection::doABarrelRoll()
{
    boost::asio::spawn(m_sock->get_io_service(),
                       boost::bind(&TcpConnection::handleConnect,
                                   shared_from_this(),
                                   _1));
}

void TcpConnection::handleConnect(boost::asio::yield_context yield)
{
    try {
        boost::asio::streambuf buffer(1024);
        boost::asio::async_read_until(*m_sock, buffer, "\n", yield);
        std::istream is(&buffer);
        std::string line;
        std::getline(is, line);

        std::string filename = getFileName(line);

        /// Whell, whell, whell. What do we have there
        if (filename.empty()) {
            std::cerr << "It's not get request\n";
            return;
        }
        if (!youWantFileOrWhat(filename)) {
            std::cerr << "There is no such file\n";
            return;
        }

        int file;
        if ((file = open(filename.c_str(), O_RDONLY)) == -1) {
            std::cerr << "Can't open this file\n";
            return;
        }
        close(file);

        std::cout << filename << std::endl;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

std::__cxx11::string TcpConnection::getFileName(std::__cxx11::string req)
{
    size_t pos = req.find("get");
    if (pos == std::string::npos)
        return std::string();
    std::string withoutGet = req.substr(pos + 4);     /// length("GET ") is 4
    std::string filenameInUrl;
    std::istringstream ist(withoutGet);
    ist >> filenameInUrl;

    std::string ret;

    for (size_t i = 0; i < filenameInUrl.length(); ++i) {
        if (filenameInUrl[i] == '%') {
            int val;
            sscanf(filenameInUrl.substr(i + 1, 2).c_str(), "%x", &val);
            ret += static_cast<char>(val);
            i += 2;
        } else if (filenameInUrl[i] == '+') {
            ret += ' ';
        } else {
            ret += filenameInUrl[i];
        }
    }

    return ret;
}

bool TcpConnection::youWantFileOrWhat(std::__cxx11::string filename)
{

    path pathToFile(filename);
    return (is_regular_file(pathToFile) and exists(pathToFile));
}
