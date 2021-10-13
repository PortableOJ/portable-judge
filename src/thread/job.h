//
// Created by keqing on 2021-10-12.
//

#ifndef JUDGE_JOB_H
#define JUDGE_JOB_H

class Job {
    void *data;
    function<void(void *data)> func;
public:
    Job(void *data, function<void(void *data)> func);

    void exec();
};

/// region define

Job::Job(void *data, function<void(void *)> func) {
    this->data = data;
    this->func = move(func);
}

void Job::exec() {
    func(data);
}

/// endregion

#endif //JUDGE_JOB_H
