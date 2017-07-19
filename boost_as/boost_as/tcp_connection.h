#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <string>

using boost::asio::ip::tcp;

class TcpConnection : public  std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(std::shared_ptr<tcp::socket> &&socket);

    void doABarrelRoll();

private:
    void handleConnect(boost::asio::yield_context yield);
    std::string getFileName(std::string req);

    bool youWantFileOrWhat(std::string filename);

    std::shared_ptr<tcp::socket> m_sock;
};

#endif // TCP_CONNECTION_H
