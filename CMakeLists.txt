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

# nlohmann_json
find_package(nlohmann_json 3.2.0 REQUIRED)

include_directories(src)

file(GLOB_RECURSE SRC
        src/cache/*.cpp
        src/cache/*.h
        src/compress/*.cpp
        src/compress/*.h
        src/db/*.cpp
        src/db/*.h
        src/file/*.cpp
        src/file/*.h
        src/config/*.cpp
        src/config/*.h
        src/filter/*.cpp
        src/filter/*.h
        src/log/*.cpp
        src/log/*.h
        src/memory/*.cpp
        src/memory/*.h
        src/recovery/*.cpp
        src/recovery/*.h
        src/table/*.cpp
        src/table/*.h
        src/utils/*.cpp
        src/utils/*.h
        src/wal/*.cpp
        src/wal/*.h
        src/main.cpp)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE pthread nlohmann_json::nlohmann_json)

if (SMALLKV-UNITTEST)
    enable_testing()
    file(GLOB_RECURSE SRC_WITHOUT_MAIN
            src/cache/*.cpp
            src/cache/*.h
            src/compress/*.cpp
            src/compress/*.h
            src/db/*.cpp
            src/db/*.h
            src/file/*.cpp
            src/file/*.h
            src/config/*.cpp
            src/config/*.h
            src/filter/*.cpp
            src/filter/*.h
            src/log/*.cpp
            src/log/*.h
            src/memory/*.cpp
            src/memory/*.h
            src/recovery/*.cpp
            src/recovery/*.h
            src/table/*.cpp
            src/table/*.h
            src/utils/*.cpp
            src/utils/*.h
            src/wal/*.cpp
            src/wal/*.h)
    file(GLOB_RECURSE SRC_TEST
            tests/*.cpp)
    add_executable(smallkv-unitest ${SRC_WITHOUT_MAIN} ${SRC_TEST})
    # 需要链接一下
    target_link_libraries(smallkv-unitest PRIVATE gtest pthread nlohmann_json::nlohmann_json)
endif (SMALLKV-UNITTEST)