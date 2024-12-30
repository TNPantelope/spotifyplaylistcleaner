cmake_minimum_required(VERSION 3.29)
project(spotifyplaylistcleaner)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)



find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

find_package(cpprestsdk REQUIRED)

add_executable(spotifyplaylistcleaner main.cpp)

include_directories(
        ${CURL_INCLUDE_DIR}
        ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(spotifyplaylistcleaner
        PRIVATE
        ${CURL_LIBRARIES}
        cpprestsdk::cpprest
        OpenSSL::SSL
        OpenSSL::Crypto
)