#include "server.h"

Server::Server(io_service &service, int port)
    : m_acceptor(service, endpoint(v4(), port))
{
    /// start_accept()
}

void Server::startAccept()
{
    /// TODO: tcpConnection
}
