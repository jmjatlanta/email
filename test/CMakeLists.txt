cmake_minimum_required(VERSION 3.0)

project(test_email)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable( test_email
    test.cpp
)

target_include_directories( test_email PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

include(GoogleTest)

target_link_libraries(test_email 
    email
    GTest::gtest_main
)
