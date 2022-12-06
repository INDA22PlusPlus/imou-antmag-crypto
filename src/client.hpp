#include <iostream>
#include <string>
#include <sodium.h>
#include <boost/asio.hpp>
#include <boost/array.hpp>


namespace client {
    void send(const boost::asio::const_buffer& data,
              const boost::array<uint8_t, crypto_aead_aes256gcm_KEYBYTES>& key,
              const std::string& ip, uint16_t port);

    void get(const boost::asio::mutable_buffer& buf,
             const boost::array<uint8_t, crypto_aead_aes256gcm_KEYBYTES>& key,
             const std::string& ip, uint16_t port);
};