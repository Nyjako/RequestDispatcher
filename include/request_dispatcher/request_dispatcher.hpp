/**
 * @file request_dispatcher.hpp
 * @author Kacper Tucholski (kacper.tucholski.kt@gmail.com)
 * @brief Manages and dispatches HTTP requests using libcurl while enforcing a specified rate limit.
 * @version 1.0.0
 * @date 2024-07-06
 * 
 * @copyright Boost Software License - Version 1.0 - August 17th, 2003
 * 
 *  Copyright (c) 2024 Kacper Tucholski
 *
 *  Permission is hereby granted, free of charge, to any person or organization
 *  obtaining a copy of the software and accompanying documentation covered by
 *  this license (the "Software") to use, reproduce, display, distribute,
 *  execute, and transmit the Software, and to prepare derivative works of the
 *  Software, and to permit third-parties to whom the Software is furnished to
 *  do so, all subject to the following:
 * 
 *  The copyright notices in the Software and this entire statement, including
 *  the above license grant, this restriction and the following disclaimer,
 *  must be included in all copies of the Software, in whole or in part, and
 *  all derivative works of the Software, unless such copies or derivative
 *  works are solely in the form of machine-executable object code generated by
 *  a source language processor.
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 *  SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 *  FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 *  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */

#ifndef REQUEST_DISPATCHER_HPP
#define REQUEST_DISPATCHER_HPP

#pragma once

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include <chrono>

#include <curl/curl.h>

#include "request_dispatcher/request_dispatcher_export.hpp"

/**
 * A note about the MSVC warning C4251:
 * This warning should be suppressed for private data members of the project's
 * exported classes, because there are too many ways to work around it and all
 * involve some kind of trade-off (increased code complexity requiring more
 * developer time, writing boilerplate code, longer compile times), but those
 * solutions are very situational and solve things in slightly different ways,
 * depending on the requirements of the project.
 * That is to say, there is no general solution.
 *
 * What can be done instead is understand where issues could arise where this
 * warning is spotting a legitimate bug. I will give the general description of
 * this warning's cause and break it down to make it trivial to understand.
 *
 * C4251 is emitted when an exported class has a non-static data member of a
 * non-exported class type.
 *
 * The exported class in our case is the class below (exported_class), which
 * has a non-static data member (m_name) of a non-exported class type
 * (std::string).
 *
 * The rationale here is that the user of the exported class could attempt to
 * access (directly, or via an inline member function) a static data member or
 * a non-inline member function of the data member, resulting in a linker
 * error.
 * Inline member function above means member functions that are defined (not
 * declared) in the class definition.
 *
 * Since this exported class never makes these non-exported types available to
 * the user, we can safely ignore this warning. It's fine if there are
 * non-exported class types as private member variables, because they are only
 * accessed by the members of the exported class itself.
 *
 * The name() method below returns a pointer to the stored null-terminated
 * string as a fundamental type (char const), so this is safe to use anywhere.
 * The only downside is that you can have dangling pointers if the pointer
 * outlives the class instance which stored the string.
 *
 * Shared libraries are not easy, they need some discipline to get right, but
 * they also solve some other problems that make them worth the time invested.
 */

enum TimeUnit
{
    MINUTES = 60,
    HOURS = 3600,
    DAYS = 86400
};

/**
 * @brief The RequestDispatcher class manages and dispatches HTTP requests using libcurl while enforcing a specified rate limit.
 * 
 */
class REQUEST_DISPATCHER_EXPORT RequestDispatcher
{
  public:
    /**
     * @brief Construct a new Request Dispatcher object
     * 
     * @param time_frame_seconds What is window for requests
     * @param request_count how many requests can be done in specyfied window
     */
    RequestDispatcher(int time_frame_seconds, int request_count);

    /**
     * @brief Destroy the Request Dispatcher object
     * 
     */
    ~RequestDispatcher();

    /**
     * @brief Queues a libcurl request for asynchronous execution.
     * 
     * @param curl request to execute
     * @return std::future<CURLcode> that the user can use to wait for and retrieve the result of the request.
     */
    auto make_request(CURL* curl) -> std::future<CURLcode>;

  private:
    REQUEST_DISPATCHER_SUPPRESS_C4251

    /**
     * @brief The duration (in seconds) of the time frame within which requests are rate-limited.
     * 
     */
    int m_time_frame_seconds;

    /**
     * @brief The maximum number of requests that can be sent within the specified time frame.
     * 
     */
    int m_max_request_count;

    /**
     * @brief A flag indicating whether the worker thread should stop processing requests.
     * 
     */
    bool m_stop_worker;

    /**
     * @brief A mutex to protect access to the request queue.
     * 
     */
    std::mutex m_queue_mutex;

    /**
     * @brief A condition variable to notify the worker thread of new requests.
     * 
     */
    std::condition_variable m_cv;

    /**
     * @brief A queue that holds pairs of CURL* handles and their corresponding std::promise<CURLcode> objects.
     * 
     */
    std::queue< std::pair<CURL*, std::promise<CURLcode>> > m_request_queue;

    /**
     * @brief A thread that processes the request queue and sends requests asynchronously.
     * 
     */
    std::thread m_worker_thread;

    /**
     * @brief The worker thread's main function.
     * Processes the request queue, sends requests, and enforces rate limits.
     */
    void process_queue();
};

#endif  // REQUEST_DISPATCHER_HPP