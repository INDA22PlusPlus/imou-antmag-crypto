#include <iostream>
#include <string>

#include <sodium.h>
#include<client.hpp>
#include<boost/asio.hpp>
#include<boost/system/error_code.hpp>
#include"server.hpp"

using namespace std;
using namespace boost::asio;



int main(int argc, char** argv) {
    if (argc == 1) { throw std::invalid_argument("Not enough arguments!"); }
    io_context ctx;

    if (std::string("--server").compare(argv[1]) == 0) {
        /* ./crypto --server IP PORT */
        //if (argc != 4) { throw std::invalid_argument("Not enought arguments!"); }
        
        /* Do server thingz */
        server S(ctx);
        S.accept();

    } else if (std::string("-get").compare(argv[1]) == 0) {
        /* ./crypto -get ID IP PORT*/
        //if (argc != 5) { throw std::invalid_argument("Not enough arguments!"); }
        
        client C(ctx);
        C.establish_connection("127.0.0.1", server::PORT);

        boost::asio::const_buffer buff("Hello World!", 13);
        boost::array<uint8_t, 32UL> A;

        C.send(buff, A, "127.0.0.1", server::PORT);
        C.close();

        /* Do client thingz - receive file */
    } else if (std::string("-send").compare(argv[1]) == 0) {
        /* Do client thingz - send encrypted file*/
    
        /* ./crypto -send FILE KEY ID PORT */
        if (argc != 6) { throw std::invalid_argument("Not enough arguments!"); }


    } else {
        throw std::invalid_argument("Non valid valid argument given");
    }

    return 0;
}