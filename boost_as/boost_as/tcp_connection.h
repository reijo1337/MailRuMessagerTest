#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <boost/enable_shared_from_this.hpp>

class TcpConnection
        : public boost::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection();
};

#endif // TCP_CONNECTION_H
