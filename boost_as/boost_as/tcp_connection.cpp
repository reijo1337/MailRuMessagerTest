#include "tcp_connection.h"

#include <boost/bind.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sys/sendfile.h>

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
        int file;
        boost::asio::streambuf buffer(1024);
        boost::asio::async_read_until(*m_sock, buffer, "\n", yield);
        std::istream is(&buffer);
        std::string line;
        std::getline(is, line);

        std::string protocol, trash;
        std::istringstream ist(line);
        ist >> trash >> m_filename >> protocol;

        m_filename = getFileName(m_filename);

        int requestCode = 200;

        /// Whell, whell, whell. What do we have there
        if (m_filename.empty()) {
            std::cerr << "It's not get request\n";
            requestCode = 400;
        } else if (!youWantFileOrWhat()) {
            std::cerr << "There is no such file\n";
            requestCode = 404;
        } else if ((file = open(m_filename.c_str(), O_RDONLY)) == -1) {
            std::cerr << "Can't  open file\n";
            requestCode = 500;
        }

        std::cout << "File request: " << m_filename << std::endl;

        std::string headers(answerHeaders(protocol, requestCode));
        std::vector<char> headersVector(headers.c_str(), headers.c_str() + headers.length() + 1);
        boost::asio::async_write(*m_sock, boost::asio::buffer(&headersVector[0], headersVector.size()), yield);

        switch (requestCode) {
        case 200:
        {
            off_t offset = 0;
            int send = 0;
            path pathToFile(m_filename);
            int length = file_size(pathToFile);
            m_sock->async_write_some(boost::asio::null_buffers(), yield);
            send = sendfile(m_sock->native(), file, &offset, length);
            if (send < 0)
                std::cerr << "Fail in sending file\n";
            break;
        }
        case 400:
        {
            const char* error400 = "<html> 400 Unable to process request  </html>";
            std::cerr << error400 << std::endl;
            boost::asio::async_write(*m_sock, boost::asio::buffer(error400, strlen(error400)), yield);
            break;
        }
        case 404:
        {
            const char* error404 = "<html> 404 File not found </html>";
            std::cerr << error404 << std::endl;
            boost::asio::async_write(*m_sock, boost::asio::buffer(error404, strlen(error404)), yield);
            break;
        }
        case 500:
        {
            const char* error500 = "<html> 500 Can't open file </html>";
            std::cerr << error500 << std::endl;
            boost::asio::async_write(*m_sock, boost::asio::buffer(error500, strlen(error500)), yield);
            break;
        }
        default:
            break;
        }

        close(file);

        closeConnect();

        std::cout << headers << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Connect exception: " << e.what() << std::endl;
    }
}

std::__cxx11::string TcpConnection::getFileName(std::__cxx11::string req)
{
    size_t pos = req.find("get");
    if (pos == std::string::npos)
        return std::string();
    std::string withoutGet = req.substr(pos + 4);     /// length("GET ") is 4

    std::string ret;

    for (size_t i = 0; i < withoutGet.length(); ++i) {
        if (withoutGet[i] == '%') {
            int val;
            sscanf(withoutGet.substr(i + 1, 2).c_str(), "%x", &val);
            ret += static_cast<char>(val);
            i += 2;
        } else if (withoutGet[i] == '+') {
            ret += ' ';
        } else {
            ret += withoutGet[i];
        }
    }

    return ret;
}

bool TcpConnection::youWantFileOrWhat()
{

    path pathToFile(m_filename);
    return (is_regular_file(pathToFile) and exists(pathToFile));
}

std::__cxx11::string TcpConnection::answerHeaders(std::__cxx11::string protocol, int code)
{
    std::ostringstream ret;

    ret << protocol;
    switch (code) {
    case 200:
        ret << " 200 OK\r\n";
        break;
    case 400:
        ret << " 400 Bad Request\r\n";
        break;
    case 404:
        ret << " 404 Not Found\r\n";
        break;
    case 500:
        ret << " 500 Internal Server Error\r\n";
        break;
    default:
        break;
    }

    ret.imbue(std::locale(ret.getloc(), new boost::posix_time::time_facet("%a, %d %b %Y %H:%M:%S GMT")));
    ret << "Server: FileServer/1.0 (Linux) \r\n"
            << "Date: " << boost::posix_time::second_clock::universal_time() << "\r\n"
            << "Connection: close\r\n";

    if (code == 200) {
        ret << fileHeaders();
    }

    return ret.str();
}

std::__cxx11::string TcpConnection::fileHeaders()
{
    std::ostringstream ret;

    size_t lastDot = m_filename.find_last_of(".");
    std::string ex = m_filename.substr(lastDot + 1);

    ret << "Content-Type: " << m_mimeTypes[ex] << "\r\n";
    ret << "Content-Disposition: attachment; filename=\"" << m_filename << "\"\r\n";
    ret << "Content-Length: " << file_size(m_filename) << "\r\n";

    return ret.str();
}

void TcpConnection::closeConnect()
{
    boost::system::error_code error = boost::asio::error::interrupted;
    for (; m_sock->is_open() && error == boost::asio::error::interrupted;)
        m_sock->close(error);
}
