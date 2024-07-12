#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "request_dispatcher/request_dispatcher.hpp"

#include <curl/curl.h>

RequestDispatcher::RequestDispatcher(int time_frame_seconds,
                                     int max_request_count)
    : m_time_frame_seconds(time_frame_seconds)
    , m_max_request_count(max_request_count)
    , m_stop_worker(false)
{
    m_worker_thread = std::thread(&RequestDispatcher::process_queue, this);
}

RequestDispatcher::~RequestDispatcher()
{
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        m_stop_worker = true;
    }
    m_cv.notify_all();
    m_worker_thread.join();
}

auto RequestDispatcher::make_request(CURL* curl) -> std::future<CURLcode>
{
    std::promise<CURLcode> promise;
    auto future = promise.get_future();

    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        m_request_queue.emplace(curl, std::move(promise));
    }
    m_cv.notify_all();

    return future;
}

void RequestDispatcher::process_queue()
{
    int request_count = 0;
    auto start_time = std::chrono::steady_clock::now();

    while (true) {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_cv.wait(lock,
                  [this] { return !m_request_queue.empty() || m_stop_worker; });

        if (m_stop_worker && m_request_queue.empty()) {
            break;
        }

        auto [curl, promise] = std::move(m_request_queue.front());
        m_request_queue.pop();
        lock.unlock();

        auto now = std::chrono::steady_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::seconds>(now - start_time);

        // if we moved away from time window we can start another one
        if (duration.count() >= m_time_frame_seconds) {
            request_count = 0;
            start_time = now;
        }

        // if limit was reached wait for clear
        if (request_count >= m_max_request_count) {
            auto wait_time =
                std::chrono::seconds(m_time_frame_seconds) - duration;
            std::this_thread::sleep_for(wait_time);

            request_count = 0;
            start_time = std::chrono::steady_clock::now();
        }

        CURLcode res = curl_easy_perform(curl);
        promise.set_value(res);

        request_count++;
    }
}