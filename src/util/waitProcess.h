//
// Created by 胡柯青 on 2022-03-22.
//

#ifndef JUDGE_WAIT_PROCESS_H
#define JUDGE_WAIT_PROCESS_H

#include "include.h"

class WaitProcess {
public:
    bool static wait(int pid, int &code, rusage *rus);
};

bool WaitProcess::wait(int pid, int &code, rusage *rus) {
    int status;
    wait4(pid, &status, 0, rus);

    /**
     * WIFEXITED 若程序正常执行完成了，则返回非 0
     * WEXITSTATUS 若程序正常执行完成了，则获取程序退出的值（一般为 0）
     * WTERMSIG 若程序没有正常执行完成，则获取退出此程序的系统错误
     */
    if (WIFEXITED(status) != 0) code = WEXITSTATUS(status);
    else code = WTERMSIG(status);
    return WIFEXITED(status) != 0;
}

#endif //JUDGE_WAIT_PROCESS_H
