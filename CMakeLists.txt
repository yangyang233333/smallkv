cmake_minimum_required(VERSION 3.16)
project(smallkv)

set(CMAKE_CXX_STANDARD 17)

#add_compile_options(-march=native -ffast-math)
#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CXX_FLAGS "-Werror -Wextra")
#set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

# spdlog
include_directories(3rdparty/spdlog/include)

include_directories(src) # 头文件搜索目录
file(GLOB_RECURSE SRC src/*.cpp) # 找出源文件
add_executable(${PROJECT_NAME} ${SRC}) # 编译成动态库

# 链接
find_library(PTHREAD pthread)
target_link_libraries(${PROJECT_NAME} ${PTHREAD})
