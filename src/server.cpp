
#include"server.hpp"
#include<iostream>

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;

server::server(io_context& _ctx) 
    : ctx(_ctx), 
      acc(_ctx, tcp::endpoint(tcp::v4(), server::PORT)){
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
    std::cout << "Server established a connection!" << std::endl;

    char buff[512];
    boost::asio::mutable_buffer boost_buff(buff, 512);
    while(true){
        try
        {
            size_t bytes = new_connection.get_socket().receive(boost_buff);
            std::cout << "We just read " << bytes << " bytes!" << std::endl;
            std::cout << std::string(buff, bytes) << std::endl;
            /* code */
        }
        catch(const boost::system::system_error& e)
        {
            std::cerr << e.what() << '\n';
            break;
        }
    }
}

void connection::create_soc(boost::asio::io_context& ctx){
    //Should deallocate upon reassignment
    this->soc_p = std::make_unique<tcp::socket>(ctx);
}

tcp::socket& connection::get_socket(){
    return *(this->soc_p);
}
