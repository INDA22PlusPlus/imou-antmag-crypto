cmake_minimum_required(VERSION 3.0.0)

project(crypto
    VERSION 1.0
    DESCRIPTION "Crypto Server for KTH assignment"
    LANGUAGES CXX)

add_executable(crypto
    main.cpp)

target_compile_options(crypto PRIVATE -Isrc/ -Wall -Wextra -g)