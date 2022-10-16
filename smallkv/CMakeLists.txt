cmake_minimum_required(VERSION 3.23)
project(smallkv)

set(CMAKE_CXX_STANDARD 14)

add_compile_options(-march=native -ffast-math)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-Werror -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

include_directories(src) # 头文件搜索目录
file(GLOB_RECURSE SRC src/*.cpp) # 找出源文件
add_library(${PROJECT_NAME} SHARED ${SRC}) # 编译成动态库

# 链接
find_library(PTHREAD pthread)
target_link_libraries(${PROJECT_NAME} ${PTHREAD})
