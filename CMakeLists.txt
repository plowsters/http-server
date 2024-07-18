# Minimum version of CMake required
cmake_minimum_required(VERSION 3.23)

# Project name
project(BugHTTPServer)

# Set the C standard
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add the executable target
add_executable(BugHTTPServer http_server.c handlers.c)

#Make debug or release build
set(CMAKE_BUILD_TYPE Debug)