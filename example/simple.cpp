#include <iostream>

#include <curl/curl.h>

#include "request_dispatcher/request_dispatcher.hpp"

auto main() -> int
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // 10 requests per minute
    RequestDispatcher dispatcher(1 * TimeUnit::MINUTES, 10);

    for (int i = 0; i < 20; ++i) {
        CURL* curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, "http://example.com");

        auto future = dispatcher.make_request(curl);

        future.wait();
        CURLcode res = future.get();

        std::cout << "Request " << i + 1 << " completed with code: " << res
                  << std::endl;

        // user is responsible for cleaning up the CURL handle
        if (res == CURLE_OK) {
            char* content_type;
            curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
            if (content_type) {
                std::cout << "Content-Type: " << content_type << std::endl;
            }
        }
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return 0;
}