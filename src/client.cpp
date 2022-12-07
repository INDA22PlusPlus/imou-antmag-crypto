
#include"client.hpp"
#include<memory>
#include<boost/make_unique.hpp>

using namespace boost::asio;
using namespace boost::system;
using namespace boost::asio::ip;

client::client(boost::asio::io_context& _ctx) : ctx(_ctx) {}

int client::establish_connection(const std::string& ip, uint16_t port){
    // Create a TCP endpoint for the server on port 80
    ip::tcp::endpoint endpoint(ip::address::from_string(ip), port);
    
    // Create a socket and connect it to the endpoint
    this->soc_p = std::make_unique<ip::tcp::socket>(this->ctx);
    
    //Connecting
    error_code err;
    this->soc_p->connect(endpoint, err);
    if(err){
        std::cerr << "Error: " << err.message() << std::endl;
        return 1;
    }
    this->connected = true;

    return 0;
}

int client::get(
    const boost::asio::mutable_buffer& buf,
    const boost::array<uint8_t, crypto_aead_aes256gcm_KEYBYTES>& key,
    const std::string& ip, uint16_t port)
{
    assert(this->connected == true);
    boost::system::error_code err;

    // Receive a response from the server
    char response[512];
    size_t bytes_read = read(*(this->soc_p), buffer(response), err);

    if(err){
        std::cerr << "Error: " << err.message() << std::endl;
    }

    // Print the response
    std::cout << "Response: " << std::string(response, bytes_read) << std::endl;
}

int client::send(
    const boost::asio::const_buffer& data,
    const boost::array<uint8_t, crypto_aead_aes256gcm_KEYBYTES>& key,
    const std::string& ip, uint16_t port)
{
    // Send a message to the server
    error_code err;
    write(*(this->soc_p), data, err);
    if(err){
        std::cerr << "Error: " << err.message() << std::endl;
        return 1;
    }

    return 0;
}

void client::close(){
    error_code err;
    this->soc_p->close(err);
    if(err){
        std::cout << "Could not close the socket, Error: " << err.message() << std::endl;
    }
}
