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
    std::string *str = static_cast<std::string *>(userp);
    str->append(static_cast<char *>(buffer), size * nmemb);
    return size * nmemb;
}

static std::string base64_encode(const std::string &in) {
    std::string out;
    static const std::string base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

    int val = 0, valb = -6;
    for (unsigned char c: in) {
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

std::string webserver() {
    std::string redirect_uri = "http://localhost:8888/callback";
    std::string scope =
            "user-read-private%20user-read-email%20user-library-read%20playlist-modify-public%20playlist-read-private%20playlist-modify-private";;

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

    // waiting for user to go authenticate
    std::cout << "waiting for auth, press enter to continue" << std::endl;
    std::cin.get();

    // after enter
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
    std::string body_parameters =
            "grant_type=authorization_code"
            "&code=" + oAuth_code +
            "&redirect_uri=http://localhost:8888/callback";


    curl = curl_easy_init();
    if (curl) {
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
        try {
            json response = json::parse(readBuffer);
            oAuth_Token = response["access_token"];

            return oAuth_Token;
        } catch (json::exception &e) {
            return "";
        }
    }
    return "";
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
                for (auto &item: response["items"]) {
                    std::string songID = item["track"]["id"];
                    LikedSongsData.push_back(songID);
                }

                // update the offset for the next batch
                offset += limit;
            } catch (json::exception e) {
                break;
            }
        } while (offset < total_songs);

        curl_slist_free_all(header);
        curl_easy_cleanup(curl);

        return LikedSongsData;
    }
    return LikedSongsData;
}

std::vector<std::string> getPlaylistData(std::string accessToken, std::string playlistID) {
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
    int limit = 50;
    int total_songs = 0;
    bool first_request = true;


    std::string playlist_id = playlistID;

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

            // in case song doesnt have id (removed from spotify but still in playlist)
            if (readBuffer.empty()) {
                std::cout << "empty response get playlistdata" << std::endl;
                break;
            }

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
                    if (!item["track"].is_null() && !item["track"]["id"].is_null()) {
                        std::string songID = item["track"]["id"];
                        playlistData.push_back(songID);
                    }
                }

                // update the offset for the next batch
                offset += limit;
            } catch (json::exception e) {
                break;
            }
        } while (offset < total_songs);

        curl_slist_free_all(header);
        curl_easy_cleanup(curl);


        return playlistData;
    }
    return playlistData;
}

std::vector<std::string> get_unique_songs(std::vector<std::string> likedsongslist, std::vector<std::string> playlistsongslist) {
    std::vector<std::string> uniqueSongs;

    /*
    for (const std::string &likedSong: likedsongslist) {
        for (const std::string &playlistSong: playlistsongslist) {
            if (likedSong == playlistSong) {
                matchingSongs.push_back(likedSong);
                break;
            }
        }
    }
    */


    for (const std::string &playlistSong: playlistsongslist) {
        bool songisLiked = false;
        for (const std::string &likedSong: likedsongslist) {
            if (playlistSong == likedSong) {
                songisLiked = true;
                break;
            }
        }
        if (!songisLiked) {
            uniqueSongs.push_back(playlistSong);
        }
    }
    return uniqueSongs;
}






std::string getUsername(std::string accessToken) {
    std::string userName;
    struct curl_slist *header = NULL;
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    bool first_request = true;

    std::string GrantType = "Authorization: Bearer ";
    std::string authBearerToken = GrantType + accessToken;

    // making our header
    header = curl_slist_append(NULL, authBearerToken.c_str());
    curl = curl_easy_init();

    if (curl) {
        do {
            readBuffer.clear();
            curl_easy_setopt(curl, CURLOPT_URL, "https://api.spotify.com/v1/me");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            res = curl_easy_perform(curl);

            if (readBuffer.empty()) {
                std::cout << "empty response, get username" << std::endl;
                break;
            }

            try {
                json response = json::parse(readBuffer);

                userName = response["display_name"];
                first_request = false;
            } catch (json::exception e) {
                break;
            }
        } while (first_request);


        curl_slist_free_all(header);
        curl_easy_cleanup(curl);

        //std::cout << readBuffer << std::endl;

        return userName;
    }
    return userName;
}


std::string create_playlist(std::string accessToken, std::string playlistName) {
    std::string new_playlistID;
    struct curl_slist *header = NULL;
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    std::string GrantType = "Authorization: Bearer ";
    std::string authBearerToken = GrantType + accessToken;

    // making our header
    header = curl_slist_append(NULL, authBearerToken.c_str());
    header = curl_slist_append(header, "Content-Type: application/json");


    std::string body_parameters = "{"
                                  "\"name\":\"" + playlistName + "\","
                                  "\"description\":\"this is a copy\","
                                  "\"public\":true"
                                  "}";


    curl = curl_easy_init();
    if (curl) {
        readBuffer.clear();

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body_parameters.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.spotify.com/v1/me/playlists");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);

        if (readBuffer.empty()) {
            std::cout << "playlist id is empty" << std::endl;
        }

        try {
            json response = json::parse(readBuffer);

            new_playlistID = response["id"];
        } catch (json::exception e) {
            std::cout << "empty response create playlist" << std::endl;
        }

        curl_slist_free_all(header);
        curl_easy_cleanup(curl);

        //std::cout << readBuffer << std::endl;

        return new_playlistID;
    }

    return new_playlistID;
}


void fill_new_playlist(std::string accessToken, std::string playlistID, std::vector<std::string> list_of_songs) {
    struct curl_slist *header = NULL;
    CURL *curl;
    CURLcode res;
    std::string readBuffer;


    std::string GrantType = "Authorization: Bearer ";
    std::string authBearerToken = GrantType + accessToken;

    header = curl_slist_append(NULL, authBearerToken.c_str());
    header = curl_slist_append(header, "Content-Type: application/json");

    curl = curl_easy_init();

    int i = 0;
    while (i < list_of_songs.size()) {
        // filling in the list of songs we boutta put in this b

        std::string body_parameters =
                "{"
                "\"uris\":[\"spotify:track:" + list_of_songs[i] + "\"]"
                "}";


        // Build URL with the right parameters
        std::string playlistUrl =
                "https://api.spotify.com/v1/playlists"
                "/" + playlistID +
                "/tracks";


        if (curl) {
            readBuffer.clear();

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body_parameters.c_str());
            curl_easy_setopt(curl, CURLOPT_URL, playlistUrl.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            res = curl_easy_perform(curl);



            i++;
        }


    }
    curl_slist_free_all(header);
    curl_easy_cleanup(curl);
    std::cout << "should be there now" << std::endl;
}

int main() {
    std::string playlistID = "1H9FE8NW6n3BObXmnjNrb7";

    //gets oauth code using webserver func, passes that into oAuth to token, stores that token in oAuthToken
    std::string oAuthToken = oAuth_to_token(webserver());

    // pass the auth token to these various funtions
    std::vector<std::string> playlistData = getPlaylistData(oAuthToken, playlistID);
    std::vector<std::string> likedSongs = getLikedSongs(oAuthToken);

    std::vector<std::string> uniqueSongs = get_unique_songs(likedSongs, playlistData);

    // get username
    std::string userName = getUsername(oAuthToken);
    std::cout << "Spotify Username: " << userName << std::endl;


    // create playlsit
    std::string playlistName = "test slist";
    std::string new_playlistID = create_playlist(oAuthToken, playlistName);
    std::cout << "new playlists id: " << new_playlistID << std::endl;


    std::string list_of_songs;
    fill_new_playlist(oAuthToken, new_playlistID, uniqueSongs);


    std::cout << "\n";

    // Za prints
    std::cout << "how many unique songs: " << uniqueSongs.size() << std::endl;

    std::cout << "how many songs in playlist: " << playlistData.size() << std::endl;

    std::cout << "how many liked songs: " << likedSongs.size() << std::endl;

    return 0;
}
