//
// Created by keqing on 2021-10-26.
//

#ifndef JUDGE_TIME_MUTEX_H
#define JUDGE_TIME_MUTEX_H

#include "include.h"

class TimeoutMutex {
    mutex m;
    condition_variable cv;
    unique_lock<mutex> lk;
public:
    TimeoutMutex();

    bool wait(int pid, unsigned long ms, int &code, rusage *rus);
};

/// region define

TimeoutMutex::TimeoutMutex() : m(), cv(), lk(m) {}

bool TimeoutMutex::wait(int pid, unsigned long ms, int &code, rusage *rus) {
    thread th([&]() {
        if (cv.wait_for(lk, chrono::seconds(ms)) == cv_status::timeout) kill(pid, SIGKILL);
    });

    int status;
    wait4(pid, &status, 0, rus);
    cv.notify_all();
    th.join();
    /**
     * WIFEXITED 若程序正常执行完成了，则返回非 0
     * WEXITSTATUS 若程序正常执行完成了，则获取程序退出的值（一般为 0）
     * WTERMSIG 若程序没有正常执行完成，则获取退出此程序的系统错误
     */
    if (WIFEXITED(status) != 0) code = WEXITSTATUS(status);
    else code = WTERMSIG(status);
    return WIFEXITED(status) != 0;
}

/// endregion

#endif //JUDGE_TIME_MUTEX_H
