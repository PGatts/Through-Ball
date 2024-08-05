cmake_minimum_required(VERSION 3.27)
project(Through_Ball)

set(CMAKE_CXX_STANDARD 14)

include_directories(/opt/homebrew/Cellar/nlohmann-json/3.11.3/include)


include_directories(/opt/homebrew/Cellar/asio/1.30.2/include)
set(SOURCE_FILES main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")

add_executable(Through_Ball
        main.cpp
        graph.h)
