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

    bool youWantFileOrWhat();

    std::string answerHeaders(std::string protocol, int code);
    std::string fileHeaders();

    void closeConnect();

    std::shared_ptr<tcp::socket> m_sock;
    std::map<std::string, std::string> m_mimeTypes = {
        {"zip", "application/zip"},
        {"txt", "text/plain"},
        {"html", "text/html"},
        {"css", "text/css"},
        {"csv", "text/csv"},
        {"js", "application/javascript"},
        {"jpg", "image/jpeg"},
        {"jpeg", "image/jpeg"},
        {"gif", "image/gif"},
        {"png", "image/png"},
        {"gif", "image/gif"},
        {"swf", "application/x-shockwave-flash"},
        {"iso", "application/octet-stream"}
    };
    std::string m_filename;
};

#endif // TCP_CONNECTION_H
