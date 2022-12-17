cmake_minimum_required(VERSION 3.16)
project(smallkv)

set(CMAKE_CXX_STANDARD 17)

#add_compile_options(-march=native -ffast-math)
#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CXX_FLAGS "-Werror -Wextra")
#set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")
option(SMALLKV-UNITTEST "build unit-test" OFF)

include_directories(3rdparty/spdlog/include)# spdlog
include_directories(src)

file(GLOB_RECURSE SRC src/*.cpp)
add_executable(${PROJECT_NAME} ${SRC})
find_library(PTHREAD pthread)
target_link_libraries(${PROJECT_NAME} ${PTHREAD})

if (SMALLKV-UNITTEST)
    enable_testing()
    file(GLOB_RECURSE SRC_WITHOUT_MAIN
            src/filter/*.cpp
            src/log/*.cpp
            src/memory/*.cpp
            src/utils/*.cpp)
    file(GLOB_RECURSE SRC_TEST tests/*.cpp)
    add_executable(smallkv-unitest ${SRC_WITHOUT_MAIN} ${SRC_TEST})
    target_link_libraries(smallkv-unitest libgtest.so)
endif (SMALLKV-UNITTEST)