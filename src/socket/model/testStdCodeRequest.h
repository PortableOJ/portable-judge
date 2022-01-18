//
// Created by 胡柯青 on 2022-01-18.
//

#ifndef JUDGE_TEST_STD_CODE_REQUEST_H
#define JUDGE_TEST_STD_CODE_REQUEST_H

#include "../request.h"

class TestStdCodeRequest : public Request {
public:
    explicit TestStdCodeRequest(id problemId);
};

/// region define

TestStdCodeRequest::TestStdCodeRequest(id problemId) : Request(Api::TestStdCodeRequest) {
    this->set(constant.id, problemId);
}

/// endregion

#endif //JUDGE_TEST_STD_CODE_REQUEST_H
