//
// Created by keqing on 2021-10-26.
//

#ifndef JUDGE_C_COMPILER_H
#define JUDGE_C_COMPILER_H

#include "../compiler.h"

class CCompiler : public Compiler {
private:
    [[nodiscard]] static bool trace(int pid);

    static void toCompile(const path &code, const string &param);

public:

    [[nodiscard]] string versionSupport() const override;

    [[nodiscard]] bool compile(const path &code, const string &param) const override;

    void collectCompileInfo(const path &code, string &result) const override;
};

/// region define

bool CCompiler::trace(int pid) {
    TimeoutMutex timeoutMutex;
    int exitCode;
    bool status = timeoutMutex.wait(pid, compileRealMaxTime, exitCode, nullptr);
    return status && exitCode == 0;
}

void CCompiler::toCompile(const path &code, const string &param) {
    rlimit timeLimit{compileMaxTime, compileMaxTime};
    rlimit memLimit{compileMaxMemory, compileMaxMemory};
    rlimit fileLimit{compileFileSize, compileFileSize};
    setrlimit(RLIMIT_CPU, &timeLimit);
    setrlimit(RLIMIT_AS, &memLimit);
    setrlimit(RLIMIT_FSIZE, &fileLimit);

    path output = code;
    output.replace_extension("");
    path compileInfo = code;
    compileInfo.replace_filename("compileError.txt");

    const char cmd[] = "/usr/bin/gcc";
    const char *const argv[] = {cmd, code.c_str(), "-O2", "-o", output.c_str(), "-Wall", "-lm",
                                "--static", param.c_str(), "-DONLINE_JUDGE", "-fmax-errors=5", nullptr};
    const char *const env[] = {"PATH=/usr/bin", nullptr};

    freopen(compileInfo.c_str(), "w", stderr);
    execve(cmd, const_cast<char *const *>(argv), const_cast<char *const *>(env));
}

string CCompiler::versionSupport() const {
    string str;
    if (doCmd("gcc --version", str)) {
        return string("C ") + to_string(str.length()) + "\n" + str + "\n";
    }
    return string();
}

bool CCompiler::compile(const path &code, const string &param) const {
    int pid = fork();
    if (pid == 0) {
        toCompile(code, param);
    } else {
        return trace(pid);
    }
    return false;
}

void CCompiler::collectCompileInfo(const path &code, string &result) const {
    path compileInfo = code;
    compileInfo.replace_filename("compileError.txt");
    ifstream input(compileInfo);
    string tmp;
    while (getline(input, tmp)) result += tmp + '\n';
}

/// endregion


#endif //JUDGE_C_COMPILER_H
