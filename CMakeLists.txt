cmake_minimum_required(VERSION 3.16)
project(smallkv LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-march=native -ffast-math)
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CXX_FLAGS "-Werror -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")
option(SMALLKV-UNITTEST "build unit-test" OFF)

include_directories(3rdparty/spdlog/include)# spdlog
include_directories(src)

file(GLOB_RECURSE SRC src/*.cpp)
add_executable(${PROJECT_NAME} ${SRC})
find_library(PTHREAD pthread)
target_link_libraries(${PROJECT_NAME} ${PTHREAD})

if (SMALLKV-UNITTEST)
    enable_testing()
    add_subdirectory(3rdparty/googletest)
endif (SMALLKV-UNITTEST)