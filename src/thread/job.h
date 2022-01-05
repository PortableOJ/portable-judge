//
// Created by 胡柯青 on 2021-10-12.
//

#ifndef JUDGE_JOB_H
#define JUDGE_JOB_H

#include "../util/countMutex.h"

class Job {
private:
    bool noDelete;

    CountMutex *countMutex;

protected:
    explicit Job(CountMutex *cm, bool noDelete = false);

    virtual void work() = 0;

public:
    virtual ~Job() = default;

    void exec();

    bool toDelete();
};

/// region define

Job::Job(CountMutex *cm, bool noDelete) : countMutex(cm), noDelete(noDelete) {}

void Job::exec() {
    work();
    if (countMutex != nullptr) {
        countMutex->trigger();
    }
}

bool Job::toDelete() {
    return !noDelete;
}

/// endregion

#endif //JUDGE_JOB_H
