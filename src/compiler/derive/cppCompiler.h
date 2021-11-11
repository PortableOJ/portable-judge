//
// Created by keqing on 2021-11-05.
//

#ifndef JUDGE_CPP_COMPILER_H
#define JUDGE_CPP_COMPILER_H

#include "../compiler.h"

class CppCompiler : public Compiler {
private:

    [[nodiscard]] static bool trace(int pid);

    static void toCompile(const path &code, const string &param);

public:

    [[nodiscard]] string versionSupport() const override;

    [[nodiscard]] bool compile(const path &code, const string &param) const override;
};

/// region define

bool CppCompiler::trace(int pid) {
    TimeoutMutex timeoutMutex;
    int exitCode;
    bool status = timeoutMutex.wait(pid, compileRealMaxTime, exitCode, nullptr);
    return status && exitCode == 0;
}

void CppCompiler::toCompile(const path &code, const string &param) {
    rlimit timeLimit{compileMaxTime, compileMaxTime};
    rlimit memLimit{compileMaxMemory, compileMaxMemory};
    rlimit fileLimit{compileFileSize, compileFileSize};
    setrlimit(RLIMIT_CPU, &timeLimit);
    setrlimit(RLIMIT_AS, &memLimit);
    setrlimit(RLIMIT_FSIZE, &fileLimit);

    path output = code;
    output.replace_extension("");
    path compileInfo = code;
    compileInfo.remove_filename();
    compileInfo /= "compileError.txt";

    const char cmd[] = "/usr/bin/g++";
    const char *const argv[] = {cmd, code.c_str(), "-O2", "-o", output.c_str(), "-Wall", "-lm",
                                "--static", param.c_str(), "-DONLINE_JUDGE", "-fmax-errors=5", nullptr};
    const char *const env[] = {"PATH=/usr/bin", nullptr};

    freopen(compileInfo.c_str(), "w", stderr);
    execve(cmd, const_cast<char *const *>(argv), const_cast<char *const *>(env));
}

string CppCompiler::versionSupport() const {
    string str;
    if (doCmd("g++ --version", str)) {
        return string("CPP ") + to_string(str.length()) + "\n" + str + "\n";
    }
    return string();
}

bool CppCompiler::compile(const path &code, const string &param) const {
    int pid = fork();
    if (pid == 0) {
        toCompile(code, param);
    } else {
        return trace(pid);
    }
    return false;
}

/// endregion

#endif //JUDGE_CPP_COMPILER_H
