cmake_minimum_required(VERSION 3.25)
project(TodoAPI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CROW_ENABLE_COMPRESSION ON)
set(CROW_ENABLE_SSL ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

add_subdirectory(external/Crow)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(SQLite3 REQUIRED)

file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(todo_api ${SOURCES})

target_link_libraries(todo_api
    PRIVATE
    Crow::Crow
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    SQLite::SQLite3
)