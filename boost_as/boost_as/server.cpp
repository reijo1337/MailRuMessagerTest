#include "server.h"
#include <iostream>

#include <boost/bind.hpp>
#include <string>
#include <memory>

#include "tcp_connection.h"

Server::Server(io_service &service, int port) :
    m_service(service), m_acceptor(service, tcp::endpoint(tcp::v4(), port))
{
    startAccept();
}

void Server::startAccept()
{
    std::shared_ptr<tcp::socket> sock(new tcp::socket(m_service));
    m_acceptor.async_accept(*sock,
                            boost::bind(&Server::handleAccept,
                                        this,
                                        sock,
                                        boost::asio::placeholders::error));
}

void Server::handleAccept(std::shared_ptr<tcp::socket> &socket, const boost::system::error_code &err_code)
{
    if (!err_code)
        try {
            std::make_shared<TcpConnection>(TcpConnection(std::move(socket)))->doABarrelRoll();
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }

    /// And again
    startAccept();
}
