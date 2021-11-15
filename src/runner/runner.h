//
// Created by keqing on 2021-11-11.
//

#ifndef JUDGE_RUNNER_H
#define JUDGE_RUNNER_H

#include "../language/__init__.h"
#include "result.h"

struct Report {
    // 毫秒
    unsigned long timeCost;
    // MB
    unsigned long memoryCost;
};

class Runner {
private:
    static unsigned long xOrShf96();

    static unsigned long maxRunningRealTime;

    void runCode(const path &code,
                 int *input, int *output, int *error,
                 unsigned long limitTime, unsigned long limitMemory,
                 const string &params,
                 unsigned long randomCode, bool allowOpenFile);

    static JudgeResultEnum trace(int pid, unsigned long randomCode, int *error, Report *report);

protected:
    virtual void addRule(const path &code, scmp_filter_ctx &ctx, function<void(int)> systemError) = 0;

    virtual void exec(const path &code, const string &params) = 0;

public:
    JudgeResultEnum run(const path &code,
                        int *input,
                        int *output,
                        int *error,
                        unsigned long limitTime,
                        unsigned long limitMemory,
                        const string &params,
                        Report *report = nullptr,
                        bool allowOpenFile = false);
};

/// region define

unsigned long Runner::maxRunningRealTime = 60000 / STD::s;

unsigned long Runner::xOrShf96() {
    static unsigned long x = 123456789, y = 362436069, z = 521288629;
    unsigned long t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;

    return z;
}

void Runner::runCode(const path &code, int *input, int *output, int *error,
                     unsigned long limitTime, unsigned long limitMemory,
                     const string &params,
                     unsigned long randomCode, bool allowOpenFile) {
    if (input != nullptr) {
        if (input[0] != -1) dup2(input[0], 0);
        if (input[1] != -1) close(input[1]);
    }

    if (output != nullptr) {
        if (output[0] != -1) close(output[0]);
        if (output[1] != -1) dup2(output[1], 1);
    }

    if (error != nullptr) {
        if (error[0] != -1) close(error[0]);
        if (error[1] != -1) dup2(error[1], 2);
    }

    auto ctxFail = [&](int syscall) {
        string str = to_string(randomCode);
        Logger::err("Seccomp rule add fail: %", syscall);
        if (error != nullptr && error[1] != -1) write(error[1], str.c_str(), str.size());
        exit(-1);
    };

    rlimit timeLimit{limitTime, limitTime + 1};
    rlimit memLimit{limitMemory, limitMemory};
    setrlimit(RLIMIT_CPU, &timeLimit);
    setrlimit(RLIMIT_AS, &memLimit);

    // strace -c ./main
    int whitelist[] = {SCMP_SYS(read), SCMP_SYS(fstat),
                       SCMP_SYS(mmap), SCMP_SYS(mprotect),
                       SCMP_SYS(munmap), SCMP_SYS(uname),
                       SCMP_SYS(arch_prctl), SCMP_SYS(brk),
                       SCMP_SYS(access), SCMP_SYS(exit_group),
                       SCMP_SYS(close), SCMP_SYS(readlink),
                       SCMP_SYS(sysinfo), SCMP_SYS(write),
                       SCMP_SYS(writev), SCMP_SYS(lseek),
                       SCMP_SYS(pread64), SCMP_SYS(clock_gettime)};
    int whitelistLength = sizeof(whitelist) / sizeof(int);
    scmp_filter_ctx ctx;
    ctx = seccomp_init(SCMP_ACT_KILL);
    if (!ctx) ctxFail(1000);
    for (int i = 0; i < whitelistLength; i++) {
        if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, whitelist[i], 0)) ctxFail(whitelist[i]);
    }
    if (allowOpenFile) {
        if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(open), 1,
                             SCMP_CMP(1, SCMP_CMP_MASKED_EQ, O_WRONLY | O_RDWR, 0)) != 0)
            ctxFail(SCMP_SYS(open));
        if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(openat), 1,
                             SCMP_CMP(2, SCMP_CMP_MASKED_EQ, O_WRONLY | O_RDWR, 0)) != 0)
            ctxFail(SCMP_SYS(openat));
    }

    this->addRule(code, ctx, ctxFail);

    if (seccomp_load(ctx)) ctxFail(10000);
    seccomp_release(ctx);
    if (error != nullptr && error[1] != -1) write(error[1], "0\n", 2);

    exec(code, params);
    ctxFail(-1);
}

JudgeResultEnum Runner::trace(int pid, unsigned long randomCode, int *error, Report *report) {
    TimeoutMutex timeoutMutex;
    int exitCode;
    rusage usage{};

    if (error != nullptr) close(error[1]);

    bool status = timeoutMutex.wait(pid, maxRunningRealTime, exitCode, &usage);
    if (report != nullptr) {
        report->timeCost = usage.ru_utime.tv_sec * 1000 + usage.ru_utime.tv_usec / 1000 +
                           usage.ru_stime.tv_sec * 1000 + usage.ru_stime.tv_usec / 1000;
        report->memoryCost = usage.ru_maxrss / 1024;
    }
    bool systemFail;
    if (error != nullptr) {
        char c;
        int code = 0;
        while (read(error[0], &c, 1)) {
            if (isdigit(c)) {
                code *= 10;
                code += c - '0';
            } else break;
        }
        systemFail = code == randomCode;
    }
    if (status)
        return exitCode == 0 ? JudgeResultEnum::Accept :
               systemFail ? JudgeResultEnum::SystemError : JudgeResultEnum::ReturnNotZero;
    else {
        switch (exitCode) {
            case SIGFPE:
                return JudgeResultEnum::ErroneousArithmeticOperation;
            case SIGSEGV:
                return JudgeResultEnum::SegmentFail;
            case SIGXCPU:
                return JudgeResultEnum::TimeLimitExceeded;
            case SIGUSR2:
            case SIGSYS:
                return JudgeResultEnum::IllegalSystemCall;
            default:
                return JudgeResultEnum::RuntimeError;
        }
    }
}

JudgeResultEnum Runner::run(const path &code,
                            int *input, int *output, int *error,
                            unsigned long limitTime, unsigned long limitMemory,
                            const string &params,
                            Report *report, bool allowOpenFile) {
    // 需要一个随机数，保证不为 0 即可
    unsigned long randomCode = xOrShf96() % 100000 + 1;
    int pid = fork();
    if (pid == 0) {
        runCode(code, input, output, error, limitTime, limitMemory, params, randomCode, allowOpenFile);
        return JudgeResultEnum::JudgeCompileError;
    } else {
        return trace(pid, randomCode, error, report);
    }
}

/// endregion

#endif //JUDGE_RUNNER_H
