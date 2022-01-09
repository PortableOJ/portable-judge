//
// Created by 胡柯青 on 2021-11-14.
//

#ifndef JUDGE_TEST_WORK_H
#define JUDGE_TEST_WORK_H

class TestWork {

public:
    TestWork(id problemId, ThreadPool *tp, SessionPool *sp);

    void start();

};

/// region define

TestWork::TestWork(id problemId, ThreadPool *tp, SessionPool *sp) {

}

void TestWork::start() {

}

/// endregion

#endif //JUDGE_TEST_WORK_H
