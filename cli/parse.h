//
// Created by qianyy on 2023/1/29.
//
#include <string>
#include <exception>
#include <utility>
#include "db/options.h"
#include "db/db.h"
#include "db/db_impl.h"

#ifndef SMALLKV_PARSE_H
#define SMALLKV_PARSE_H
namespace smallkv::cli {
    struct CMD_TYPE {
        CMD_TYPE() = delete;

        static constexpr const char *EXIT = "exit";
        static constexpr const char *EMPTY = "";
        static constexpr const char *SET = "set";
        static constexpr const char *GET = "get";
        static constexpr const char *DEL = "del";
    };

    struct ExecStatus {
        ExecStatus() = delete;

        // 命令解析状态
        static constexpr const char *SYNTAX_ERR = "(error) ERR syntax error";
        static constexpr const char *UNKNOWN_CMD = "(error) ERR unknown command";
        static constexpr const char *TOOLONG_CMD = "(error) ERR too long command";

        // DB执行状态
        static constexpr const char *OK = "OK";
        static constexpr const char *NOTFOUND = "(nil)";
        static constexpr const char *EXEC_FAILED = "(error) ERR exec failed";
        static constexpr const char *SUCCESS_DELETED_ONE = "(integer) 1";
    };

    // 只作为演示、debug用
    class ParseCMD {
    public:
        explicit ParseCMD(Options options = MakeOptionsForDebugging());

        ~ParseCMD() {
            db_holder->Close();
        }

        // 解析cmd并且返回执行结果
        std::string ParseAndExec(std::string &cmd_);

        // 去除前后的空格
        static std::string Trim(std::string &cmd);

    private:
        Options options_;
        WriteOptions wOp{};
        ReadOptions rOp{};
        std::shared_ptr<DB> db_holder;
    };
}
#endif //SMALLKV_PARSE_H
