//
// Created by Tamim Norani on 30/12/2024.
//

#include "auth_server.h"

using namespace web::http;
using namespace web::http::experimental::listener;


AuthServer::AuthServer(utility::string_t url) : listener(url) {
    listener.support(methods::GET,
        std::bind(&AuthServer::handle_callback, this, std::placeholders::_1));
}

void AuthServer::handle_callback(web::http::http_request request) {
    auto query = web::uri::split_query(request.request_uri().query());

    if (query.find(U("code")) != query.end()) {
        auth_code = query[U("code")];
        request.reply(status_codes::OK, U("Authorization successful, you can close window and go back"));
    } else {
        request.reply(status_codes::BadRequest, U("auth code not received"));
    }
}

void AuthServer::start() {
    listener.open().wait();
}

void AuthServer::stop() {
    listener.close().wait();
}

utility::string_t AuthServer::get_auth_code() const {
    return auth_code;
}