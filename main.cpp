#include <iostream>
#include <string>
#include <curl/curl.h>
#include "auth_server.h"
#include "json.hpp" // for JSON parsing

using json = nlohmann::json;

std::string client_id = "a24ea60b20244fd299abb97fc9ce2ce5";
std::string client_secret = "dd43497448614f0dbd9469726b50f673";

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




std::vector<std::string> getLikedSongs(std::string accessToken) {
    struct curl_slist *header = NULL;
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    std::vector<std::string> LikedSongsData;

    std::string GrantType = "Authorization: Bearer ";
    std::string authBearerToken = GrantType + accessToken;

    // making our header
    header = curl_slist_append(NULL, authBearerToken.c_str());


    curl = curl_easy_init();

    int offset = 0;
    int limit = 50;
    int total_songs = 0;
    bool first_request = true;


    if (curl) {
        do {
            readBuffer.clear();
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

            // json parse bs
            try {
                json response = json::parse(readBuffer);

                // get the total number of liked songs on the first request
                if (first_request) {
                    total_songs = response["total"];
                    first_request = false;
                }
                // pull out song ids from the items array
                for (auto& item : response["items"]) {
                    std::string songID = item["track"]["id"];
                    LikedSongsData.push_back(songID);
                }

                // update the offset for the next batch
                offset += limit;

            } catch(json::exception e) {
                break;
            }

        } while (offset < total_songs);

        curl_slist_free_all(header);
        curl_easy_cleanup(curl);

        return LikedSongsData;
    }
}


std::vector<std::string> getPlaylistData(std::string accessToken) {
    struct curl_slist *header = NULL;

    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    std::vector<std::string> playlistData;

    std::string GrantType = "Authorization: Bearer ";
    std::string authBearerToken = GrantType + accessToken;


    // making our header
    header = curl_slist_append(NULL, authBearerToken.c_str());


    int offset = 0;
    int limit = 1;
    int total_songs = 0;
    bool first_request = true;

    std::string playlist_id = "09uKR1njKVo82C7vTpnQXu";

    curl = curl_easy_init();
    if (curl) {
        do {
            readBuffer.clear();

            // Build URL with the right parameters
            std::string playlistUrl =
                    "https://api.spotify.com/v1/playlists"
                    "/" + playlist_id +
                    "/tracks?"
                    "offset=" + std::to_string(offset) +
                    "&limit=" + std::to_string(limit);
            curl_easy_setopt(curl, CURLOPT_URL, playlistUrl.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            res = curl_easy_perform(curl);

            // json parse bs
            try {
                json response = json::parse(readBuffer);

                // get the total number of liked songs on the first request
                if (first_request) {
                    total_songs = response["total"];
                    first_request = false;
                }
                // pull out song ids from the items array
                for (auto &item: response["items"]) {
                    std::string songID = item["track"]["id"];
                    playlistData.push_back(songID);
                }

                // update the offset for the next batch
                offset += limit;
            } catch (json::exception e) {
                break;
            }
        } while (offset < total_songs);

        curl_slist_free_all(header);
        curl_easy_cleanup(curl);

        // test print entire response
        //std::cout << readBuffer << std::endl;

        return playlistData;
    }
}

std::string webserver() {

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
    std::vector<std::string> playlistData = getPlaylistData(oAuthToken);
    std::vector<std::string> likedSongs = getLikedSongs(oAuthToken);

    // Za prints
    std::cout << playlistData.size() << std::endl;
    for (auto playlist_songID : playlistData) {
        std::cout << playlist_songID << std::endl;
    }

    std::cout << "/////////////////////////////////// " << std::endl;

    std::cout << likedSongs.size() << std::endl;
    //for (auto& liked_songID : likedSongs) {
        //std::cout << liked_songID << std::endl;
    //}

    return 0;
}