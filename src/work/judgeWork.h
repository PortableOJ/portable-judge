//
// Created by 胡柯青 on 2021-11-14.
//

#ifndef JUDGE_JUDGE_WORK_H
#define JUDGE_JUDGE_WORK_H

#include "../thread/__init__.h"
#include "../file/__init__.h"
#include "../compiler/__init__.h"
#include "../runner/__init__.h"
#include "../socket/__init__.h"

class JudgeWork {
private:
    bool fail;
    CountMutex cm;

    id solutionId{};
    id problemId{};
    Language *language{};
    string judgeName;
    int testNum{};
    unsigned long timeLimit{};
    unsigned long memoryLimit{};

    SessionPool *sessionPool;
    ThreadPool *threadPool;

    bool init();

public:

    JudgeWork(id solutionId, ThreadPool *tp, SessionPool *sp);

    void start();
};

/// region define

/// endregion

#endif //JUDGE_JUDGE_WORK_H
