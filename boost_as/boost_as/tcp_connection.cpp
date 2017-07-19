#include "tcp_connection.h"

#include <boost/bind.hpp>
#include <iostream>

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

        std::string what = getFileName(line);
        std::cout << what << std::endl;
      }
      catch (std::exception& e)
      {
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
