cmake_minimum_required(VERSION 3.20)

include(cmake/prelude.cmake)

project(
    request_dispatcher
    VERSION 1.1.4
    DESCRIPTION "Short description"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)
include(cmake/libs.cmake)

# ---- Declare library ----

add_library(
    request_dispatcher_request_dispatcher
    source/request_dispatcher.cpp
)
add_library(request_dispatcher::request_dispatcher ALIAS request_dispatcher_request_dispatcher)

include(GenerateExportHeader)
generate_export_header(
    request_dispatcher_request_dispatcher
    BASE_NAME request_dispatcher
    EXPORT_FILE_NAME export/request_dispatcher/request_dispatcher_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(request_dispatcher_request_dispatcher PUBLIC REQUEST_DISPATCHER_STATIC_DEFINE)
endif()

set_target_properties(
    request_dispatcher_request_dispatcher PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME request_dispatcher
    OUTPUT_NAME request_dispatcher
)

target_include_directories(
    request_dispatcher_request_dispatcher ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    request_dispatcher_request_dispatcher SYSTEM
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(request_dispatcher_request_dispatcher PUBLIC cxx_std_17)

target_link_libraries(request_dispatcher_request_dispatcher PRIVATE CURL::libcurl)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${request_dispatcher_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()

# ---- Developer mode ----

if(NOT request_dispatcher_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of request_dispatcher"
  )
endif()

include(cmake/dev-mode.cmake)
