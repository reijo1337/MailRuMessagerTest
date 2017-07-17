#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include
using namespace boost::asio;
using namespace boost::asio::ip::tcp;

class Server
{
public:
    Server(io_service &service, int port);

private:
    void startAccept();

    acceptor m_acceptor;
};

#endif // SERVER_H
