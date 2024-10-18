cmake_minimum_required(VERSION 3.0)

project(test_email)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

find_package(Boost REQUIRED COMPONENTS system filesystem log log_setup iostreams)
find_package(OpenSSL REQUIRED)
find_package(Curl REQUIRED)
find_package(ZLIB REQUIRED)

add_executable( test_email
    test.cpp
)

target_include_directories( test_email PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ib_helper/src/util
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

include(GoogleTest)

target_link_libraries(test_email 
    email
    mailio
    ib_helper
    ${OPENSSL_LIBRARIES}
    GTest::gtest_main
    Boost::filesystem
    Boost::system
    Boost::log
    Boost::log_setup
    Boost::iostreams
    ZLIB::ZLIB
)

if(HH_DATELIB)
target_link_libraries(test_email 
    date
    CURL::libcurl
)
endif(HH_DATELIB)
