#include <iostream>
#include <string>

#include <sodium.h>


int main(int argc, char** argv) {
    if (argc == 1) { throw std::invalid_argument("Not enough arguments!"); }

    if (std::string("--server").compare(argv[1]) == 0) {
        /* Do server thingz */
        
        /* ./crypto --server IP PORT */
        if (argc != 4) { throw std::invalid_argument("Not enought arguments!"); }




    } else if (std::string("-get").compare(argv[1]) == 0) {
        /* Do client thingz - receive file */
    
        /* ./crypto -get ID IP PORT*/
        if (argc != 5) { throw std::invalid_argument("Not enough arguments!"); }
    } else if (std::string("-send").compare(argv[1]) == 0) {
        /* Do client thingz - send encrypted file*/
    
        /* ./crypto -send FILE KEY ID PORT */
        if (argc != 6) { throw std::invalid_argument("Not enough arguments!"); }


    } else {
        throw std::invalid_argument("Non valid valid argument given");
    }

    return 0;
}