#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>
#include <boost/array.hpp> 
#include <boost/make_unique.hpp>

#include "client.hpp"


using namespace boost::asio;
using namespace boost::system;
using namespace boost::asio::ip;


client::client(boost::asio::io_context& _ctx, const std::string& key) : ctx(_ctx) {

    /* Just copy the key */
    std::copy(key.begin(), key.end(), std::back_inserter(key));

    //Setting the nonce, yes i know, this should not be fixed
    randombytes_buf(nonce, sizeof(nonce));
}

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

int client::get_file(int id) {
    assert(this->connected == true);
    
    std::ofstream out_file;
    std::string _str_id;
    uint8_t* decoded_buffer;
    unsigned long long decoded_len;
    int status;
    

    out_file.open(std::to_string(id) + std::string(".data"),
                    std::ios::out | std::ios::binary);

    const_buffer type("\x01", 1);
    this->soc_p->send(type);
    /* Place a 0 behind the number to string if the id < 10 */
    _str_id = (id < 10) ? "0" : ""; 
    _str_id.append(std::to_string(id));
    this->soc_p->send(buffer(_str_id.data(), 2));
    
    std::vector<uint8_t> encoded_buffer;

    while (true) {
        boost::array<uint8_t, 1024> buf;
        /* Get the file from the server and write it to the local file */
        try {
            std::size_t bytes = this->soc_p->receive(buffer(buf));
            std::copy(buf.begin(), buf.end(), std::back_inserter(encoded_buffer));
        } catch(const boost::system::system_error &e) {
            break;
        }
    }

    /* Decode the data */
    decoded_buffer = new uint8_t[encoded_buffer.size() - crypto_aead_aes256gcm_ABYTES];
    status = crypto_aead_aes256gcm_decrypt(
        decoded_buffer,
        &decoded_len,
        NULL,
        encoded_buffer.data(),
        encoded_buffer.size(),
        NULL,
        0,
        this->nonce,
        this->key.data()
    );

    if (status == -1) {
        std::cerr<<"file with id: "<<id<<" cannot be verified!!"<<std::endl;
        return 1;
    }

    /* Write to the output file and close */
    out_file.write((const char*)decoded_buffer, decoded_len);
    out_file.close();

    delete decoded_buffer;

    return 0;
}

int client::send_file(int file_id) {
    assert(this->connected == true);

    std::ifstream file;
    std::ofstream out_file;
    std::streampos file_size;
    uint8_t *data, *cipher;
    unsigned long long cipher_len;


    file.open(std::to_string(file_id) + std::string(".data"),
                std::ios::in | std::ios::binary);
    file.seekg(0, std::ios::end);
    file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    file.close();

    data = new uint8_t[file_size];
    cipher = new uint8_t[file_size + crypto_aead_aes256gcm_ABYTES];

    /* Encrypt as needed */
    crypto_aead_aes256gcm_encrypt(
        cipher, &cipher_len, 
        data, 
        file_size,
        nullptr,
        0,
        NULL,
        this->nonce,
        this->key.data()
    );

    /* ID for server on what client is trying to do */
    const_buffer type("\0x00", 1);
    this->soc_p->send(type);
    this->soc_p->send(buffer(cipher, cipher_len));

    return 0;
}

int check_integrity() {
    
}

//true if passed
// bool client::verify(int id, const unsigned char encrypted[CIPHER_TEXT_LEN]){
//     unsigned char correct[CIPHER_TEXT_LEN];
//     unsigned char* p = hash_mapping[id].hash;
//     memcpy(correct, p, CIPHER_TEXT_LEN);
    
//     for(int i = 0; i < (int) CIPHER_TEXT_LEN; i++){
//         if(encrypted[i] != correct[i]) return false;
//     }
//     return true;
// }

void client::close(){
    error_code err;
    this->soc_p->close(err);
    if(err){
        std::cout << "Could not close the socket, Error: " << err.message() << std::endl;
    }
}
