//
// Created by 胡柯青 on 2021-11-11.
//

#ifndef JUDGE_RUNNER_H
#define JUDGE_RUNNER_H

#include "../language/__init__.h"
#include "result.h"

struct Report {
    // ms
    unsigned long timeCost;
    // KB
    unsigned long memoryCost;
};

class Runner {
public:

    static void init();

private:

    static unsigned long maxRunningRealTime;

    /**
     * 运行代码
     * @param code 代码路径，非可执行文件路径
     * @param input 设定的输入文件句柄
     * @param output 设定的标准输出文件句柄
     * @param error 设定的错误输出文件句柄
     * @param limitTime 时间限制（s）
     * @param limitMemory 内存限制（B）
     * @param params 额外的参数
     * @param randomCode 随机数值
     * @param allowOpenFile 是否允许打开文件
     */
    void runCode(const path &code,
                 int *input, int *output, int *error,
                 unsigned long limitTime, unsigned long limitMemory,
                 const string &params, bool allowOpenFile) const;

    static JudgeResultEnum trace(int pid, int *error, Report *report);

protected:
    virtual void addRule(const path &code, scmp_filter_ctx &ctx, function<void(int)> systemError) const = 0;

    virtual void exec(const path &code, const string &params) const = 0;

public:
    JudgeResultEnum run(const path &code,
                        int *input,
                        int *output,
                        int *error,
                        unsigned long limitTime,
                        unsigned long limitMemory,
                        const string &params,
                        Report *report = nullptr,
                        bool allowOpenFile = false) const;

    virtual ~Runner() = 0;
};

/// region define

unsigned long Runner::maxRunningRealTime = 60000 / STD::s;

void Runner::init() {
    int tmp = Env::ctx()->getInt(constant.runningRealMaxTime);
    if (tmp != 0) {
        Runner::maxRunningRealTime = tmp / STD::s;
    }
}

void Runner::runCode(const path &code, int *input, int *output, int *error,
                     unsigned long limitTime, unsigned long limitMemory,
                     const string &params, bool allowOpenFile) const {
    if (input != nullptr) {
        if (input[0] != -1) dup2(input[0], STDIN_FILENO);
        if (input[1] != -1) close(input[1]);
    }

    if (output != nullptr) {
        if (output[0] != -1) close(output[0]);
        if (output[1] != -1) dup2(output[1], STDOUT_FILENO);
    }

    if (error != nullptr) {
        if (error[0] != -1) close(error[0]);
        if (error[1] != -1) dup2(error[1], STDERR_FILENO);
    }

    auto ctxFail = [&](int syscall) {
        Logger::err("Seccomp rule add fail: %", syscall);
        if (error != nullptr && error[1] != -1) write(error[1], "1\n", 2);
        exit(1);
    };
#ifdef __linux__
    /**
     *  + 1 是为了当程序超时的时候，仅仅越过软条件(soft limit)而没有越过硬条件
     *  这样目标程序会受到 SIGXCPU 这个信号而不是 SIGKILL 这个信号
     *
     *  @see https://man7.org/linux/man-pages/man2/setrlimit.2.html
     */
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
#endif
    if (error != nullptr && error[1] != -1) write(error[1], "0\n", 2);

    exec(code, params);
    ctxFail(-1);
}

JudgeResultEnum Runner::trace(int pid, int *error, Report *report) {
    TimeoutMutex timeoutMutex;
    int exitCode;
    rusage usage{};

    if (error != nullptr) close(error[1]);

    bool status = timeoutMutex.wait(pid, maxRunningRealTime, exitCode, &usage);
    if (report != nullptr) {
        report->timeCost = usage.ru_utime.tv_sec * 1000 + usage.ru_utime.tv_usec / 1000 +
                           usage.ru_stime.tv_sec * 1000 + usage.ru_stime.tv_usec / 1000;
        report->memoryCost = usage.ru_maxrss;
    }
    bool systemFail;
    if (error != nullptr) {
        int code = readInt(error[0]);
        systemFail = code != 0;
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
                            Report *report, bool allowOpenFile) const {
    int pid = fork();
    if (pid == 0) {
        runCode(code, input, output, error, limitTime, limitMemory, params, allowOpenFile);
        return JudgeResultEnum::SystemError;
    } else {

        if (input != nullptr && input[0] != -1) {
            Logger::trace("close %", input[0]);
            close(input[0]);
        }

        if (output != nullptr && output[1] != -1) {
            Logger::trace("close %", input[0]);
            close(output[1]);
        }

        if (error != nullptr && error[1] != -1) {
            Logger::trace("close %", input[0]);
            close(error[1]);
        }

        return trace(pid, error, report);
    }
}

Runner::~Runner() = default;

/// endregion

#endif //JUDGE_RUNNER_H
