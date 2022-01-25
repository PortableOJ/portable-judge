//
// Created by 胡柯青 on 2021-11-14.
//

#ifndef JUDGE_CPP_RUNNER_H
#define JUDGE_CPP_RUNNER_H

#include "../runner.h"

class CppRunner : public Runner {
protected:
    void addRule(const path &code, scmp_filter_ctx &ctx, function<void(int)> systemError) const override;

    void exec(const path &code, const string &params) const override;

public:
    ~CppRunner() override;
};

/// region define

void CppRunner::addRule(const path &code, scmp_filter_ctx &ctx, function<void(int)> systemError) const {
//        if (chroot(code.parent_path().c_str()) == -1) {
//            systemError(SCMP_SYS(chroot));
//        }
//        if (chdir("/") == -1) {
//            systemError(SCMP_SYS(chdir));
//        };
#ifdef __linux__
    if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 0,
                         SCMP_A0(SCMP_CMP_EQ, (scmp_datum_t) code.c_str())))
        systemError(SCMP_SYS(execve));
#endif
}

void CppRunner::exec(const path &code, const string &params) const {
    const char *const argv[] = {code.c_str(), params.c_str(), nullptr};
    const char *const env[] = {"PATH=/", nullptr};
    execve(code.c_str(), const_cast<char *const *>(argv), const_cast<char *const *>(env));
}

CppRunner::~CppRunner() = default;

/// endregion

#endif //JUDGE_CPP_RUNNER_H
