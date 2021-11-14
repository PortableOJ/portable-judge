//
// Created by keqing on 2021-11-14.
//

#ifndef JUDGE_CPP_RUNNER_H
#define JUDGE_CPP_RUNNER_H

#include "../runner.h"

class CppRunner : public Runner {
private:
    path codeFile;
protected:
    void addRule(const path &code, scmp_filter_ctx &ctx, function<void(int)> systemError) override {
        codeFile = code;
        codeFile.replace_extension("");

//        if (chroot(workDir.c_str()) == -1) {
//            systemError(SCMP_SYS(chroot));
//        }
//        if (chdir("/") == -1) {
//            systemError(SCMP_SYS(chdir));
//        };

        if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 0,
                             SCMP_A0(SCMP_CMP_EQ, (scmp_datum_t) codeFile.c_str())))
            systemError(SCMP_SYS(execve));
    }

    void exec(const path &code) override {
        const char *const argv[] = {codeFile.c_str(), nullptr};
        const char *const env[] = {"PATH=/", nullptr};
        execve(codeFile.c_str(), const_cast<char *const *>(argv), const_cast<char *const *>(env));
    }
};

#endif //JUDGE_CPP_RUNNER_H
