//
// Created by 胡柯青 on 2021-10-24.
//

#ifndef JUDGE_COUNT_MUTEX_H
#define JUDGE_COUNT_MUTEX_H

#include "include.h"

/**
 * 阻塞当前线程，并等待所有事务的完成
 */
class CountMutex {
    mutex m{};
    int data;
    condition_variable cv{};
public:
    explicit CountMutex(int t);

    void wait();

    void trigger();

    void reset(int t);
};

/// region define

CountMutex::CountMutex(int t) : data(t) {}

void CountMutex::wait() {
    unique_lock <mutex> lk(m);
    cv.wait(lk, [&] { return data == 0; });
    lk.unlock();
}

void CountMutex::trigger() {
    lock_guard <mutex> lockGuard(m);
    data--;
    cv.notify_all();
}

void CountMutex::reset(int t) {
    data = 0;
    cv.notify_all();
    data = t;
}

/// endregion

#endif //JUDGE_COUNT_MUTEX_H
