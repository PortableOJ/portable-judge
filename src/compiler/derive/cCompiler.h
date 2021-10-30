//
// Created by keqing on 2021-10-26.
//

#ifndef JUDGE_C_COMPILER_H
#define JUDGE_C_COMPILER_H

#include "../compiler.h"

class CCompiler : public Compiler {
private:

    [[nodiscard]] static bool trace(int pid);

    static void toCompile(const string &workDir, const string &fileName, const string &param);

public:

    CCompiler();

    [[nodiscard]] string versionSupport() const override;

    [[nodiscard]] bool compile(const string &workDir, const string &fileName, const string &param) const override;
};

/// region define

bool CCompiler::trace(int pid) {
    TimeoutMutex timeoutMutex;
    int exitCode;
    bool status = timeoutMutex.wait(pid, compileRealMaxTime, exitCode, nullptr);
    return status && exitCode == 0;
}

void CCompiler::toCompile(const string &workDir, const string &fileName, const string &param) {
    chdir(workDir.data());

    rlimit timeLimit{compileMaxTime, compileMaxTime};
    rlimit memLimit{compileMaxMemory, compileMaxMemory};
    rlimit fileLimit{compileFileSize, compileFileSize};
    setrlimit(RLIMIT_CPU, &timeLimit);
    setrlimit(RLIMIT_AS, &memLimit);
    setrlimit(RLIMIT_FSIZE, &fileLimit);

    string codeFile = fileName + ".c";
    const string &outputFile = fileName;

    const char cmd[] = "/usr/bin/gcc";
    const char *const argv[] = {cmd, codeFile.c_str(), "-O2", "-o", outputFile.c_str(), "-Wall", "-lm",
                                "--static", param.c_str(), "-DONLINE_JUDGE", "-fmax-errors=5", nullptr};
    const char *const env[] = {"PATH=/usr/bin", nullptr};

    string compileInfo = "compileError.txt";
    freopen(compileInfo.c_str(), "w", stderr);
    execve(cmd, const_cast<char *const *>(argv), const_cast<char *const *>(env));
}

CCompiler::CCompiler() : Compiler(constant.c) {}

string CCompiler::versionSupport() const {
    string str;
    if (doCmd("gcc --version", str)) {
        return string("C ") + to_string(str.length()) + "\n" + str + "\n";
    }
    return string();
}

bool CCompiler::compile(const string &workDir, const string &fileName, const string &param) const {
    int pid = fork();
    if (pid == 0) {
        toCompile(workDir, fileName, param);
    } else {
        return trace(pid);
    }
    return false;
}

/// endregion


#endif //JUDGE_C_COMPILER_H
