//
// Created by 胡柯青 on 2021-10-26.
//

#ifndef JUDGE_TIME_MUTEX_H
#define JUDGE_TIME_MUTEX_H

#include "include.h"
#include "waitProcess.h"

class TimeoutMutex {
    mutex m;
    condition_variable cv;
    unique_lock<mutex> lk;
public:
    TimeoutMutex();

    bool wait(int pid, unsigned long s, int &code, rusage *rus);
};

/// region define

TimeoutMutex::TimeoutMutex() : m(), cv(), lk(m) {}

bool TimeoutMutex::wait(int pid, unsigned long s, int &code, rusage *rus) {
    thread th([&]() {
        if (cv.wait_for(lk, chrono::seconds(s)) == cv_status::timeout) kill(pid, SIGKILL);
    });

    bool flag = WaitProcess::wait(pid, code, rus);
    cv.notify_all();
    th.join();

    return flag;
}

/// endregion

#endif //JUDGE_TIME_MUTEX_H
