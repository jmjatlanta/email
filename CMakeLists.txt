cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

###
# A library to handle creating email messages
# attempting to be cross-platform using thunderbird on Linux
# and MAPI on windows
###

project(email VERSION 1.0.0 LANGUAGES CXX)

add_subdirectory(test)

add_library( email
    email.cpp
)

#target_link_libraries(SteveDavis PRIVATE Qt6::Widgets)

