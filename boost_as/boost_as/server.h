#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>

using namespace boost::asio;
using boost::asio::ip::tcp;

class Server
{
public:
    Server(io_service &service, int port);

private:
    void startAccept();
    void handleAccept(std::shared_ptr<tcp::socket> &socket, const boost::system::error_code& err_code);

    io_service &m_service;
    tcp::acceptor m_acceptor;
};

#endif // SERVER_H
