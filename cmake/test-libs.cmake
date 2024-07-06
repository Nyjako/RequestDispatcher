include(FetchContent)

message(STATUS "Fetching test libs:")


message(STATUS "- GTest")
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)


if (WIN32)
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif (WIN32)

message(STATUS "Making GTest avaiable.")
FetchContent_MakeAvailable(googletest)