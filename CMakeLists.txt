cmake_minimum_required(VERSION 3.29)
project(spotifyplaylistcleaner)

include_directories(/usr/local/opt/curl/include/curl/curl.h)

set(CMAKE_CXX_STANDARD 20)

set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)

add_executable(spotifyplaylistcleaner main.cpp)

include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(spotifyplaylistcleaner ${CURL_LIBRARIES})