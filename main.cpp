#include <iostream>
#include <string>
#include <curl/curl.h>



// function to process the data we get
static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
    std::string *str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(buffer), size * nmemb);
    return size * nmemb;
}

// need to set curl header to null
struct curl_slist *headers=NULL;


int main()
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    // making our header
    headers = curl_slist_append(NULL, "Content-Type: application/x-www-form-urlencoded");
    
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

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


        // uses our function to process the data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
        // stores the data in readBuffer
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // frees all headers
        //curl_slist_free_all(headers);

        // actually performs the request
        res = curl_easy_perform(curl);


        curl_easy_cleanup(curl);


        // prints
        std::cout << readBuffer << std::endl;
    }







    return 0;
}