cmake_minimum_required(VERSION 3.14)

cmake_policy(SET CMP0135 NEW)

project(request_dispatcherTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)
include(../cmake/test-libs.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(request_dispatcher REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_executable(request_dispatcher_test source/request_dispatcher_test.cpp)
target_link_libraries(request_dispatcher_test PRIVATE request_dispatcher::request_dispatcher)
target_link_libraries(request_dispatcher_test PRIVATE CURL::libcurl)
target_link_libraries(request_dispatcher_test PRIVATE gtest_main)
target_compile_features(request_dispatcher_test PRIVATE cxx_std_17)

add_test(NAME request_dispatcher_test COMMAND request_dispatcher_test)
# TODO: Add more tests

# ---- End-of-file commands ----

add_folders(Test)
