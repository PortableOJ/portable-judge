//
// Created by 胡柯青 on 2022-01-07.
//

#ifndef JUDGE_TEST_TEST_H
#define JUDGE_TEST_TEST_H

#include "../request.h"

class TestTestRequest : public Request {
public:
    explicit TestTestRequest(id problemId);
};

/// region define

TestTestRequest::TestTestRequest(id problemId) : Request(Api::TestTestRequest) {
    this->set(constant.id, problemId);
}

/// endregion

#endif //JUDGE_TEST_TEST_H
