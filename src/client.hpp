#include <iostream>
#include <string>
#include <sodium.h>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/array.hpp>


class client{
    std::unique_ptr<boost::asio::ip::tcp::socket> soc_p;
    bool connected = false;
    boost::asio::io_context& ctx;
    boost::array<uint8_t, crypto_aead_aes256gcm_KEYBYTES> key;
    unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES];

public:

    client(boost::asio::io_context& _ctx, const std::string& key);

    int establish_connection(const std::string& ip, uint16_t port);
    
    int send_file(int file_id);
    
    int get_file(int file_id);

    int check_integrity();

    //bool verify(int id, const unsigned char encrypted[CIPHER_TEXT_LEN]);

    void close();
};