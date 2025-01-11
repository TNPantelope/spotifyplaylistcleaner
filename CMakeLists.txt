cmake_minimum_required(VERSION 3.29)

# Only use vcpkg and Windows-specific settings on Windows
if(WIN32)
    set(CMAKE_TOOLCHAIN_FILE "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake")
    add_definitions(-D_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS)
    set(CURL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/curl/include")
    set(CURL_LIBRARY "${CMAKE_SOURCE_DIR}/include/curl/lib/libcurl.lib")
endif()

project(spotifyplaylistcleaner)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)

# Find required packages
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(cpprestsdk REQUIRED)

# Define executable
add_executable(spotifyplaylistcleaner
        main.cpp
        auth_server.cpp
        auth_server.h)

# Include directories (Windows-specific curl includes)
if(WIN32)
    include_directories(
            ${CURL_INCLUDE_DIR}
            ${OPENSSL_INCLUDE_DIR}
    )
endif()

# Link libraries
target_link_libraries(spotifyplaylistcleaner
        PRIVATE
        ${CURL_LIBRARIES}
        cpprestsdk::cpprest
        OpenSSL::SSL
        OpenSSL::Crypto
)