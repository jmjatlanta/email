cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) # forces generation of .lib

###
# A library to handle creating email messages
# attempting to be cross-platform using thunderbird on Linux
# and MAPI on windows
###

project(email VERSION 1.0.0 LANGUAGES CXX)

find_package(Boost REQUIRED COMPONENTS log log_setup system filesystem iostreams)
find_package(OpenSSL REQUIRED)
find_package(Curl REQUIRED)

add_subdirectory(test)

add_library( email
    email.cpp
)

target_include_directories(email PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../mailio/include
    ${CMAKE_BINARY_DIR}/mailio
    ${OPENSSL_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../ib_helper/src/util
)

target_link_libraries(email PRIVATE 
    ib_helper
    dfp
    mailio
    Boost::filesystem
    Boost::iostreams
    Boost::log
    Boost::log_setup
)

if (HH_DATELIB)
target_link_libraries(email PRIVATE 
    date
)
endif(HH_DATELIB)

