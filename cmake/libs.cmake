message(STATUS "Looking for CURL")


include(FindCURL)
find_package(CURL REQUIRED)

if(CURL_FOUND)
  message(STATUS "Found CURL version: ${CURL_VERSION_STRING}")
  message(STATUS "Using CURL include dir(s): ${CURL_INCLUDE_DIRS}")
  message(STATUS "Using CURL lib(s): ${CURL_LIBRARIES}")
else()
  message(NOTICE "Could not find CURL trying to fetch")
  include(FetchContent)

  FetchContent_Declare(curl
    URL https://curl.se/download/curl-8.8.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP true
    OVERRIDE_FIND_PACKAGE # this one
  )

  FetchContent_MakeAvailable(curl)

  find_package(curl) # probably with CONFIG REQUIRED
endif()

# All following targets should search these directories for headers
include_directories( 
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CURL_INCLUDE_DIRS}
)