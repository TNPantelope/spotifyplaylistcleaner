#include <iostream>
#include <string>
#include <curl/curl.h>



// helper function to process the data we curl up
static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
    std::string *str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(buffer), size * nmemb);
    return size * nmemb;
}

// need to set curl header to null
struct curl_slist *headerContentType=NULL;
struct curl_slist *headerAuthToken=NULL;

std::string getAccessToken() {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    std::string accessToken;

    // making our header
    headerContentType = curl_slist_append(NULL, "Content-Type: application/x-www-form-urlencoded");

    // id and secret
    char *client_id_secret=
        "grant_type=client_credentials"
        "&client_id=a24ea60b20244fd299abb97fc9ce2ce5"
        "&client_secret=dd43497448614f0dbd9469726b50f673";

    // initializing curl > if successfull
    curl = curl_easy_init();
    if(curl) {

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, client_id_secret);
        curl_easy_setopt(curl, CURLOPT_URL, "https://accounts.spotify.com/api/token");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerContentType);

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
    CURL *curl;
    CURLcode res;
    std::string LikedSongsData;

    std::string AuthBearer = "Authorization: Bearer ";
    std::string authBearerToken = AuthBearer + accessToken;


    // making our header
    headerAuthToken = curl_slist_append(NULL, authBearerToken.c_str());


    // initializing curl > if successfull
    curl = curl_easy_init();
    if(curl) {

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.spotify.com/v1/me/tracks");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerAuthToken);


        // uses our function to process the data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
        // stores the data in readBuffer
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &LikedSongsData);

        // actually performs the request
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);


        return LikedSongsData;
    }
}

std::string getPlaylistData(std::string accessToken) {
    CURL *curl;
    CURLcode res;
    std::string playlistData;

    std::string AuthBearer = "Authorization: Bearer ";
    std::string authBearerToken = AuthBearer + accessToken;


    // making our header
    headerAuthToken = curl_slist_append(NULL, authBearerToken.c_str());


    // initializing curl > if successfull
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.spotify.com/v1/playlists/09uKR1njKVo82C7vTpnQXu");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerAuthToken);

        // uses our function to process the data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
        // stores the data in readBuffer
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &playlistData);

        // actually performs the request
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);


        return playlistData;
    }
}


int main()
{
    /* CREATING NEW TOKEN EACH TIME PROGRAM RUNS, FIX? IDK*/



    // get the access token
    std::string accessToken = getAccessToken();
    // passing token and getting playlist good but dont want in liked list
    std::string playlistData = getPlaylistData(accessToken);
    std::string likedSongs = getLikedSongs(accessToken);


    //std::cout << accessToken << std::endl;
    std::cout << playlistData << std::endl;
    std::cout << likedSongs << std::endl;




    return 0;
}