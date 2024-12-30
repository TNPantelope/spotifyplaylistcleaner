//
// Created by Tamim Norani on 30/12/2024.
//

#pragma once
#include <cpprest/http_listener.h>
#include <string>

class AuthServer {
public:
    AuthServer(utility::string_t url);
    void start();
    void stop();
    utility::string_t get_auth_code() const;

private:
    web::http::experimental::listener::http_listener listener;
    utility::string_t auth_code;
    void handle_callback(web::http::http_request request);
};