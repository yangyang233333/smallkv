cmake_minimum_required(VERSION 3.16)
project(smallkv)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-march=native -ffast-math)
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CXX_FLAGS "-Werror -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

option(SMALLKV-UNITTEST "build unit-test" ON)

# gtest
find_package(GTest REQUIRED)
include_directories(${GTest_INCLUDE_DIRS})

# spdlog
find_package(spdlog REQUIRED)
include_directories(${spdlog_INCLUDE_DIRS})

# RapidJSON
find_package(RapidJSON REQUIRED)
include_directories(${RapidJSON_INCLUDE_DIRS})

include_directories(src)

file(GLOB_RECURSE SRC src/*.cpp)
add_executable(${PROJECT_NAME} ${SRC})

if (SMALLKV-UNITTEST)
    enable_testing()
    file(GLOB_RECURSE SRC_WITHOUT_MAIN
            src/cache/*.cpp
            src/config/*.cpp
            src/filter/*.cpp
            src/log/*.cpp
            src/memory/*.cpp
            src/utils/*.cpp)
    file(GLOB_RECURSE SRC_TEST tests/*.cpp)
    add_executable(smallkv-unitest ${SRC_WITHOUT_MAIN} ${SRC_TEST})
    # 需要链接一下
    target_link_libraries(smallkv-unitest gtest pthread)
endif (SMALLKV-UNITTEST)