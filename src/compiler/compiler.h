//
// Created by 胡柯青 on 2021-10-26.
//

#ifndef JUDGE_COMPILER_H
#define JUDGE_COMPILER_H

#include "../thread/__init__.h"

class Compiler {
protected:
    static bool doCmd(const char *cmd, string &res) ;

public:
    static unsigned long compileMaxTime;
    static unsigned long compileRealMaxTime;
    static unsigned long compileMaxMemory;
    static unsigned long compileFileSize;

    [[nodiscard]] virtual bool compile(const path &code, const string &param) const = 0;

    virtual void collectCompileInfo(const path &code, string &result) const = 0;
};

unsigned long Compiler::compileMaxTime = 30000 / STD::s;        // s
unsigned long Compiler::compileRealMaxTime = 60000 / STD::s;    // s
unsigned long Compiler::compileMaxMemory = 2048 * STD::MB;      // B
unsigned long Compiler::compileFileSize = 1024 * STD::MB;       // B

bool Compiler::doCmd(const char *cmd, string &res) {
    FILE *pipe = popen(cmd, "r");
    if (!pipe)
        return false;

    char data[128];
    while (fscanf(pipe, "%s", data) != EOF) {
        res += data;
    }
    pclose(pipe);
    res = to_string(res.length()) + '\n' + res + '\n';
    return true;
}

#endif //JUDGE_COMPILER_H
