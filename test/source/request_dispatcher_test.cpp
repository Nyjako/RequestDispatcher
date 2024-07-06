#include "request_dispatcher/request_dispatcher.hpp"
#include <gtest/gtest.h>
#include <curl/curl.h>
#include <chrono>
#include <thread>


class RequestDispatcherTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    void TearDown() override
    {
        curl_global_cleanup();
    }

    RequestDispatcher dispatcher{60, 5}; // 5 requests per minute
};


// A simple helper function to set up a CURL handle
auto create_curl_handle(const std::string& url) -> CURL *
{
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    return curl;
}


// Test for making a single request
TEST_F(RequestDispatcherTest, SingleRequest)
{
    CURL* curl = create_curl_handle("http://example.com");
    auto future = dispatcher.make_request(curl);
    future.wait();
    CURLcode res = future.get();

    EXPECT_EQ(res, CURLE_OK);

    curl_easy_cleanup(curl);
}


// Test for making multiple requests within the rate limit
TEST_F(RequestDispatcherTest, MultipleRequestsWithinLimit)
{
    std::vector<std::future<CURLcode>> futures;
    for (int i = 0; i < 5; ++i)
    {
        CURL* curl = create_curl_handle("http://example.com");
        futures.push_back(dispatcher.make_request(curl));
    }

    for (auto& future : futures)
    {
        future.wait();
        CURLcode res = future.get();
        EXPECT_EQ(res, CURLE_OK);
    }

    for (auto& future : futures)
    {
        CURL* curl = create_curl_handle("http://example.com");
        curl_easy_cleanup(curl);
    }
}


// Test for exceeding the rate limit
TEST_F(RequestDispatcherTest, ExceedRateLimit)
{
    std::vector<std::future<CURLcode>> futures;
    for (int i = 0; i < 7; ++i)
    {
        CURL* curl = create_curl_handle("http://example.com");
        futures.push_back(dispatcher.make_request(curl));
    }

    // Wait for the first 5 requests to complete
    for (int i = 0; i < 5; ++i)
    {
        futures[i].wait();
        CURLcode res = futures[i].get();
        EXPECT_EQ(res, CURLE_OK);
    }

    // The next 2 requests should be delayed until the next time frame
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Assuming the time frame is 1 second for testing

    for (int i = 5; i < 7; ++i)
    {
        futures[i].wait();
        CURLcode res = futures[i].get();
        EXPECT_EQ(res, CURLE_OK);
    }

    for (auto& future : futures)
    {
        CURL* curl = create_curl_handle("http://example.com");
        curl_easy_cleanup(curl);
    }
}


// Test for handling invalid requests
TEST_F(RequestDispatcherTest, InvalidRequest)
{
    CURL* curl = curl_easy_init();
    auto future = dispatcher.make_request(curl);
    future.wait();
    CURLcode res = future.get();

    EXPECT_NE(res, CURLE_OK); // Expect an error for an invalid request

    curl_easy_cleanup(curl);
}


auto main(int argc, char **argv) -> int
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
