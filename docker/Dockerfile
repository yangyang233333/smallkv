# 测试镜像
FROM ubuntu:22.04

# optional
RUN cd /etc/apt/ \
    && echo "">sources.list \
    && echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ jammy main restricted universe multiverse">>sources.list \
    && echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ jammy-updates main restricted universe multiverse">>sources.list \
    && echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ jammy-backports main restricted universe multiverse">>sources.list \
    && echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ jammy-security main restricted universe multiverse">>sources.list \
    && cd ~

RUN apt update \
    && apt upgrade -y \
    && apt install vim cmake make git g++ gcc -y \
    && git clone https://github.com/gabime/spdlog.git && cd spdlog && mkdir build && cd build && cmake .. && make -j && make install && cd ~ \
    && git clone https://github.com/google/googletest && cd googletest && mkdir build && cd build && cmake .. && make -j && make install && cd ~ \
    && git clone https://github.com/nlohmann/json && cd json && mkdir build && cd build && cmake .. && make -j && make install && cd ~ \
    && git clone https://github.com/abseil/abseil-cpp.git && cd abseil-cpp && mkdir build && cd build && cmake .. && make -j && make install && cd ~ \
    && rm -rf spdlog googletest json abseil-cpp

