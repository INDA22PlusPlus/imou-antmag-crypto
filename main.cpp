#include <iostream>
#include <string>
#include <vector>

#include <sodium.h>
#include <client.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include "server.hpp"

using namespace std;
using namespace boost::asio;


int main(int argc, char** argv) {

    if (argc == 1) { throw std::invalid_argument("Not enough arguments!"); }
    io_context ctx;

    if (std::string("--server").compare(argv[1]) == 0) {
        /* ./crypto --server PORT */
        if (argc != 3) { throw std::invalid_argument("Not enought arguments!"); }
        

        /* Do server thingz, can always ctrl+c */
        server S(ctx, std::atoi(argv[2]));
        while (true) {
            S.accept();
        }

    } else if (std::string("-get").compare(argv[1]) == 0) {
        /* ./crypto -get ID IP PORT*/
        if (argc != 5) { throw std::invalid_argument("Not enough arguments!"); }
        
        uint16_t port = std::atoi(argv[4]);

        client C(ctx);
        C.establish_connection(std::string(argv[3]), port);

        boost::asio::const_buffer buff("Hello World!", 13);
        boost::array<uint8_t, 32UL> A;

        C.send(buff, A, argv[3], port);
        C.close();

        /* Do client thingz - receive file */
    } else if (std::string("-send").compare(argv[1]) == 0) {
        /* Do client thingz - send encrypted file*/
    
        /* ./crypto -send FILE IP PORT */
        if (argc != 6) { throw std::invalid_argument("Not enough arguments!"); }

    } else if (std::string("-check").compare(argv[1]) == 0) {
        /* ./crypto -check IP PORT */
        if (argc != 4) { throw std::invalid_argument("Not enough arguments!"); }

        uint16_t port = std::atoi(argv[3]);

        client C(ctx);
        C.establish_connection(std::string(argv[2]), port);

        boost::array<uint8_t, 32UL> A;
        boost::asio::const_buffer buff("\x02", 2);
        C.send(buff, A, argv[2], port);
        C.close();


    } else {
        throw std::invalid_argument("Non valid valid argument given");
    }

    return 0;
}