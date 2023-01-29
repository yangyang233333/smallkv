//
// Created by qianyy on 2023/1/29.
//
#include <iostream>
#include <algorithm>
#include "parse.h"
#include "db/db_impl.h"

namespace smallkv::cli {
    ParseCMD::ParseCMD(Options options) :
            options_(std::move(options)), db_holder(std::make_shared<DB>(options_)) {}

    std::string ParseCMD::ParseAndExec(std::string &cmd_) {
        auto cmd = Trim(cmd_);
        if (cmd == CMD_TYPE::EMPTY) {
            return "";
        } else if (cmd == CMD_TYPE::EXIT) {
            db_holder->Close();
            std::terminate();
        } else {
            if (cmd.size() < 3) {
                return std::string(ExecStatus::UNKNOWN_CMD) + ": " + cmd;
            } else if (cmd.size() > 1000) {
                return ExecStatus::TOOLONG_CMD;
            }
            // 格式必须为op key [val]
            auto op = cmd.substr(0, 3);
            if (op == CMD_TYPE::SET) {
                if (std::count(cmd.begin(), cmd.end(), ' ') != 2) {
                    return ExecStatus::SYNTAX_ERR;
                }
                int first_space = static_cast<int>(cmd.find_first_of(' ')); // 第一个空格的位置
                int last_space = static_cast<int>(cmd.find_last_of(' '));   // 最后一个空格的位置
                auto key = cmd.substr(
                        first_space + 1, last_space - first_space - 1);
                auto val = cmd.substr(last_space + 1);

                if (db_holder->Put(wOp, key, val) != Status::Success) {
                    return ExecStatus::EXEC_FAILED;
                }
                return ExecStatus::OK;
            } else if (op == CMD_TYPE::GET) {
                if (std::count(cmd.begin(), cmd.end(), ' ') != 1) {
                    return ExecStatus::SYNTAX_ERR;
                }
                int first_space = static_cast<int>(cmd.find_first_of(' '));
                auto key = cmd.substr(first_space + 1);

                // todo: value内存泄漏
                auto value = new std::string();

                if (db_holder->Get(rOp, key, value) != Status::Success) {
                    return ExecStatus::EXEC_FAILED;
                }
                if (value->empty()) {
                    return ExecStatus::NOTFOUND;
                }
                return *value;
            } else if (op == CMD_TYPE::DEL) {
                if (std::count(cmd.begin(), cmd.end(), ' ') != 1) {
                    return ExecStatus::SYNTAX_ERR;
                }
                int first_space = static_cast<int>(cmd.find_first_of(' '));
                auto key = cmd.substr(first_space + 1);
                if (db_holder->Delete(wOp, key) != Status::Success) {
                    return ExecStatus::EXEC_FAILED;
                }
                return ExecStatus::SUCCESS_DELETED_ONE;
            } else {
                return std::string(ExecStatus::UNKNOWN_CMD) + ": " + op;
            }
        }
    }

    std::string ParseCMD::Trim(std::string &cmd) {
        cmd.erase(0, cmd.find_first_not_of(' ')); // 找到第一个不在args中出现的字符下标
        cmd.erase(cmd.find_last_not_of(' ') + 1);   // 找到最后一个不在args中出现的字符的下标
        return cmd;
    }
}