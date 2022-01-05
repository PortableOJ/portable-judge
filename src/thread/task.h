//
// Created by 胡柯青 on 2021-10-22.
//

#ifndef JUDGE_TASK_H
#define JUDGE_TASK_H

#include "job.h"
#include "../util/__init__.h"

class Task : public Job {

    void *data;

    function<void(void *data)> func{};

protected:
    void work() override;

public:
    Task(void *data, function<void(void *data)> &&func, CountMutex *cm = nullptr);

    ~Task() override;
};

/// region define

void Task::work() {
    func(data);
}

Task::Task(void *data, function<void(void *)> &&func, CountMutex *cm) : Job(cm), data(data), func(move(func)) {}

Task::~Task() = default;

/// endregion

#endif //JUDGE_TASK_H
