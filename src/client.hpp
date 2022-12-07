#include <iostream>
#include <string>
#include <sodium.h>
#include <boost/asio.hpp>
#include<boost/system/error_code.hpp>
#include <boost/array.hpp>

class client{
    std::unique_ptr<boost::asio::ip::tcp::socket> soc_p;
    bool connected = false;
    boost::asio::io_context& ctx;

public:

    client(boost::asio::io_context& _ctx);

    int establish_connection(const std::string& ip, uint16_t port);
    
    int send(const boost::asio::const_buffer& data,
              const boost::array<uint8_t, crypto_aead_aes256gcm_KEYBYTES>& key,
              const std::string& ip, uint16_t port);
    
    int get(const boost::asio::mutable_buffer& buf,
             const boost::array<uint8_t, crypto_aead_aes256gcm_KEYBYTES>& key,
             const std::string& ip, uint16_t port);

    void close();
};