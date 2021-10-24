//
// Created by keqing on 2021-10-12.
//

#ifndef JUDGE_JOB_H
#define JUDGE_JOB_H

#include "../util/countMutex.h"

class Job {
private:
    CountMutex *countMutex;

protected:
    explicit Job(CountMutex *cm);

    virtual void work() = 0;

public:
    virtual ~Job() = default;

    void exec();
};

/// region define

Job::Job(CountMutex *cm) : countMutex(cm) {}

void Job::exec() {
    work();
    if (countMutex != nullptr) {
        countMutex->trigger();
    }
}

/// endregion

#endif //JUDGE_JOB_H
