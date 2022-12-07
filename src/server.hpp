#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

class connection{
    std::unique_ptr<boost::asio::ip::tcp::socket> soc_p = nullptr;
public:
    void create_soc(boost::asio::io_context& ctx);
    boost::asio::ip::tcp::socket& get_socket();
};

class server{
public:
    const static int MAX_CONNECTIONS = 1;
    const static int PORT = 1337;

private:
    boost::asio::io_context& ctx;
    boost::asio::ip::tcp::acceptor acc;
    connection connections[server::MAX_CONNECTIONS];
    int connection_cnt = 0;
public:
    server(boost::asio::io_context& _ctx);

    int accept();
    void accept_handler(const boost::system::error_code& error);

};
