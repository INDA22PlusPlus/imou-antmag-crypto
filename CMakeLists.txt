cmake_minimum_required(VERSION 3.0.0)

project(crypto
    VERSION 1.0
    DESCRIPTION "Crypto Server for KTH assignment"
    LANGUAGES CXX)

add_executable(crypto
    main.cpp
    src/client.cpp
    src/server.cpp)

target_link_directories(crypto PRIVATE /usr/local/lib)
target_include_directories(crypto PRIVATE /usr/local/include)
target_compile_options(crypto PRIVATE -Isrc/ -Wall -Wextra -g)
target_link_libraries(crypto boost_system)
target_link_libraries(crypto sodium)