cmake_minimum_required(VERSION 3.25)
project(TodoAPI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Crow REQUIRED)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

add_executable(todo_api
    src/main.cpp
)

target_link_libraries(todo_api
    PRIVATE
    Crow::Crow
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    pthread
)