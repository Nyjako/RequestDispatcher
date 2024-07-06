# RequestDispatcher

`RequestDispatcher` is a C++ library that manages and dispatches HTTP requests using `libcurl`, enforcing a specified rate limit. This library allows users to queue requests and send them asynchronously, ensuring that the number of requests within a specified time frame does not exceed the allowed limit.

## Features

- Rate-limited request dispatching
- Asynchronous request handling
- User-controlled request cleanup

## Requirements

- C++17 or later
- CMake 3.14 or later
- libcurl
- Google Test (for running tests)

# Building and installing

See the [BUILDING](BUILDING.md) document.

# Usage

## Example

Here's an example of how to use the RequestDispatcher library in your project:

```cpp
#include "request_dispatcher/request_dispatcher.hpp"
#include <curl/curl.h>
#include <iostream>

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

        std::cout << "Request " << i + 1 << " completed with code: " << res << std::endl;

        // user is responsible for cleaning up the CURL handle
        if (res == CURLE_OK) {
            char* content_type;
            curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
            if(content_type)
            {
                std::cout << "Content-Type: " << content_type << std::endl;
            }
        }
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return 0;
}
```

## API

`RequestDispatcher`

- Constructor:
    ```cpp
    RequestDispatcher(int time_frame_seconds, int max_request_count);
    ```
    Initializes the dispatcher with a specified time frame (in seconds) and a maximum request count for that time frame.

- Destructor:
    ```cpp
    ~RequestDispatcher();
    ```
    Cleans up resources and stops the worker thread gracefully.

- make_request:
    ```cpp
    std::future<CURLcode> make_request(CURL* curl);
    ```
    Queues a libcurl request for asynchronous execution. Returns a std::future<CURLcode> that the user can use to wait for and retrieve the result of the request.

# Contributing

See the [CONTRIBUTING](CONTRIBUTING.md) document.

# Licensing

This project is licensed under the **Boost Software License 1.0** License - see the [LICENSE](LICENSE) file for details.
