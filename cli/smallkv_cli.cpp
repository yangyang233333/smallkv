//
// Created by qianyy on 2023/1/29.
//
#include <string>
#include <iostream>
#include "print.h"
#include "parse.h"

using namespace smallkv;
using namespace smallkv::cli;

int main() {
    PrintWelcome();
    std::string cmd;
    Options options = MakeOptionsForDebugging();
    ParseCMD parser(options);
    while (true) {
        PrintStart();
        getline(std::cin, cmd);
        PrintResult(parser.ParseAndExec(cmd));
        cmd.clear();
    }
    return 0;
}