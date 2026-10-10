cmake_minimum_required(VERSION 3.16)

project(QtTwoWindows VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

add_executable(QtTwoWindows
    main.cpp
    SenderWindow.h
    SenderWindow.cpp
    ReceiverWindow.h
    ReceiverWindow.cpp
)

target_link_libraries(QtTwoWindows PRIVATE
    Qt6::Widgets
    Qt6::Network
)

if(MSVC)
    target_compile_options(QtTwoWindows PRIVATE /W4)
else()
    target_compile_options(QtTwoWindows PRIVATE -Wall -Wextra -Wpedantic)
endif()
