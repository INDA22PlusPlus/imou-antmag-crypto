
#include <iostream>
#include <fstream>
#include <vector>
#include <sodium.h>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/filesystem.hpp>

#include "merkle_tree.hpp"
#include "server.hpp"

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;
using namespace boost::filesystem;


static void _save_file(tcp::socket &socket) {
    /* Create server file directory if not exist already */
    if (!exists(current_path() / "server_files")) {
        create_directory("server_files");
    }

    char file_id[3];
    file_id[2] = '\0';

    assert(2 == socket.receive(buffer(&file_id, 2)));
    
    std::string file_name = std::string("server_files/") + std::string(file_id) +\
                             std::string(".data");
    std::ofstream file;
    file.open(file_name, std::ios::binary | std::ios::out);

    /* Read all file data from the client */
    while (true) {
        uint8_t buf[__CLIENT_RECEIVE_SIZE];
        try 
        {
            std::size_t bytes = socket.receive(buffer(buf, __CLIENT_RECEIVE_SIZE));
            file.write((const char*)&buf[0], bytes);
        }
        catch(const boost::system::system_error& e)
        {
            break;
        }
    }

    file.close();
    socket.close();
}

static void _get_file(tcp::socket& socket) {
    /* Only 2 bytes to encode the file id to fetch in ascii */
    char buf[3];
    /* null char, treat as a string for std::atoi */
    std::memset(&buf, 0x00, 3);
    socket.receive(buffer(&buf, 2));

    /* Convert from ascii string to int */
    uint16_t file_id = std::atoi(buf);
    /* Ish secures that no files outside the directory are accessed */
    std::string filename = std::to_string(file_id) + std::string(".data");
    /* If the file doesn't exist just close the socket */
    if (!exists(current_path() / "server_files" / filename)) {
        socket.close();
        return;
    }

    std::ifstream file;
    file.open(std::string("server_files/") + filename);
    /* Read file in chunks and send in chunks */
    while (!file.eof()) {
        uint8_t buf[__CLIENT_RECEIVE_SIZE];
        file.read((char*)&buf[0], __CLIENT_RECEIVE_SIZE);
        socket.send(buffer(&buf[0], file.gcount()));
    }
    file.close();
    socket.close();
}

static void _calc_hash(tcp::socket& socket) {

    path pth = current_path() / "server_files";

    /* If no files on server send 0 as empty identifier */
    if (!exists(pth)) {
        socket.send(buffer("\x00", 2));
        socket.close();
        return;
    }

    std::vector<std::string> hashes;
    directory_iterator it{pth};
    for (const directory_entry entry : it) {
        std::ifstream file;
        file.open(std::string("server_files/") + entry.path().string(),
                    std::ios::in | std::ios::binary);

        /* Storing the raw binary hex */
        boost::array<uint8_t, crypto_hash_sha256_BYTES> out;
        /* Storing the hexadecimal representation of the hash */
        std::string hash;
        /* Hexadecimal format, 1 byte is represented by 2 */
        hash.reserve(crypto_hash_sha256_BYTES*2);

        /* New multipart hash initialization */
        crypto_hash_sha256_state state;
        crypto_hash_sha256_init(&state);

        uint8_t buf[__CLIENT_RECEIVE_SIZE];
        while (!file.eof()) {
            file.read((char*)&buf[0], __CLIENT_RECEIVE_SIZE);
            std::size_t bytes = file.gcount();

            crypto_hash_sha256_update(&state, &buf[0], bytes);
        }

        /* Output the sha256 hash for this file's content */
        crypto_hash_sha256_final(&state, out.data());
        /* Convert the sha256 hash into a hex representation to work with */
        boost::algorithm::hex(out.begin(), out.end(), std::back_inserter(hash));

        /* Append the hash to the list */
        hashes.push_back(hash);

        file.close();
    }

    /* Calculate the top hash and send it to the client */
    std::string top_hash = calc_top_hash(hashes);
    socket.send(buffer(top_hash, top_hash.size()));
    socket.close();
}


server::server(io_context& _ctx, uint16_t port) 
    : ctx(_ctx), 
      acc(_ctx, tcp::endpoint(tcp::v4(), port)){
}

int server::accept(){
    connection& new_connection = this->connections[this->connection_cnt];
    new_connection.create_soc(this->ctx);
    
    error_code err;
    this->acc.accept(new_connection.get_socket(), err);
    if(err){
        std::cerr << "Error: " << err.message() << std::endl;
        return 1;
    }

    uint8_t type;
    new_connection.get_socket().receive(buffer(&type, 1));

    switch (type) {
        case 0x00:
            /* Saves the client's uploaded file */
            /* Closes the client socket */
            _save_file(new_connection.get_socket());
            break;
        case 0x01:
            /* Returns the file from the server given the file id */
            /* Closes the client socket */
            _get_file(new_connection.get_socket());
            break; 
        case 0x02:
            /* Calculates the top hash of Markle 'tree' on the server's files */
            /* Closes the client socket */
            _calc_hash(new_connection.get_socket());
            break;
        default:
            new_connection.get_socket().close();
    }

}


void connection::create_soc(boost::asio::io_context& ctx){
    //Should deallocate upon reassignment
    this->soc_p = std::make_unique<tcp::socket>(ctx);
}

tcp::socket& connection::get_socket(){
    return *(this->soc_p);
}
