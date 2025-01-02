#include <iostream>
#include <string>
#include <curl/curl.h>
#include "auth_server.h"
#include "json.hpp" // for JSON parsing

using json = nlohmann::json;

// helper function to process the data we curl up
static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
    std::string *str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(buffer), size * nmemb);
    return size * nmemb;
}


static std::string base64_encode(const std::string &in) {
    std::string out;
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    int val = 0, valb = -6;
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}

std::string getAccessToken() {

    struct curl_slist *header = NULL;

    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    std::string accessToken;

    // making our header
    header = curl_slist_append(NULL, "Content-Type: application/x-www-form-urlencoded");

    // id and secret
    char *client_id_secret=
        "grant_type=client_credentials"
        "&client_id=a24ea60b20244fd299abb97fc9ce2ce5"
        "&client_secret=dd43497448614f0dbd9469726b50f673";


    curl = curl_easy_init();
    if(curl) {

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, client_id_secret);
        curl_easy_setopt(curl, CURLOPT_URL, "https://accounts.spotify.com/api/token");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);

        // uses our function to process the data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
        // stores the data in readBuffer
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // actually performs the request
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        accessToken = readBuffer.substr(17, 115);

        return accessToken;
    }

}


std::string getLikedSongs(std::string accessToken) {

    struct curl_slist *header = NULL;
    CURL *curl;
    CURLcode res;
    std::string LikedSongsData;
    std::string readBuffer;

    std::string GrantType = "Authorization: Bearer ";
    std::string authBearerToken = GrantType + accessToken;

    // making our header
    header = curl_slist_append(NULL, authBearerToken.c_str());

    bool got_entire_list = false;

    curl = curl_easy_init();
    while (curl && !got_entire_list) {

        int offset = 0;
        int limit = 50;
        int i = 0;

        //building endpoint url for limit and offset
        std::string likedsongsurl =
            "https://api.spotify.com/v1/me/tracks?"
            "offset=" + std::to_string(offset) +
            "&limit=" + std::to_string(limit);

        curl_easy_setopt(curl, CURLOPT_URL, likedsongsurl.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        curl_slist_free_all(header);
        curl_easy_cleanup(curl);




        LikedSongsData = readBuffer;
        return LikedSongsData;

        i++;
        offset = (50 * i);
    }
}

std::string getPlaylistData(std::string accessToken) {

    struct curl_slist *header = NULL;

    CURL *curl;
    CURLcode res;
    std::string playlistData;

    std::string GrantType = "Authorization: Bearer ";
    std::string authBearerToken = GrantType + accessToken;


    // making our header
    header = curl_slist_append(NULL, authBearerToken.c_str());



    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.spotify.com/v1/playlists/09uKR1njKVo82C7vTpnQXu");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &playlistData);

        res = curl_easy_perform(curl);
        curl_slist_free_all(header);
        curl_easy_cleanup(curl);

        return playlistData;
    }

}

std::string webserver() {

    std::string client_id = "a24ea60b20244fd299abb97fc9ce2ce5";
    std::string redirect_uri = "http://localhost:8888/callback";
    std::string scope = "user-read-private%20user-read-email%20user-library-read";

    // creating the server
    AuthServer server(U("http://localhost:8888/callback"));
    server.start();

    // building the url
    std::string auth_url =
        "https://accounts.spotify.com/authorize?"
        "client_id=" + client_id +
        "&response_type=code"
        "&redirect_uri=" + redirect_uri +
        "&scope=" + scope;

    std::cout << "open this url:\n" << auth_url << std::endl;

    // waiting for user to authenticate
    std::cout << "waiting for auth, press enter to get code" << std::endl;
    std::cin.get();

    // after user presses enter
    auto received_oAuth_code = server.get_auth_code();
    if (received_oAuth_code.empty()) {
        std::cout << "didnt get a code" << std::endl;
    }

    server.stop();

    return received_oAuth_code;
}

std::string oAuth_to_token(std::string oAuth_code) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    std::string oAuth_Token;



    // making our headers
    std::string client_id = "a24ea60b20244fd299abb97fc9ce2ce5";
    std::string client_secret = "dd43497448614f0dbd9469726b50f673";
    std::string credentials = client_id + ":" + client_secret;

    // Create the Authorization header with Basic auth
    std::string auth_header = "Authorization: Basic " + base64_encode(credentials);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, auth_header.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");


    // body parameters
    std::string body_parameters=
        "grant_type=authorization_code"
        "&code=" + oAuth_code +
        "&redirect_uri=http://localhost:8888/callback";


    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body_parameters.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, "https://accounts.spotify.com/api/token");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // uses our function to process the data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
        // stores the data in readBuffer
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // actually performs the request
        res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        // extract the token from readbuffer
        //std::cout << "Response: " << readBuffer << std::endl;
        oAuth_Token = readBuffer.substr(17, 278);

        return oAuth_Token;
    }
}

int main()
{
    //gets oauth code using webserver func, passes that into oAuth to token, stores that token in oAuthToken
    std::string oAuthToken = oAuth_to_token(webserver());

    // pass the auth token to these various funtions
    std::string playlistData = getPlaylistData(oAuthToken);
    std::string likedSongs = getLikedSongs(oAuthToken);

    // Za prints
    std::cout << "playlist data" << playlistData << std::endl;
    std::cout << "liked songs" << likedSongs << std::endl;

    return 0;
}