//
// Created by 胡柯青 on 2022-01-18.
//

#ifndef JUDGE_TEST_INFO_REQUEST_H
#define JUDGE_TEST_INFO_REQUEST_H

#include "../request.h"

class TestInfoRequest : public Request {
public:
    explicit TestInfoRequest(id problemId);
};

/// region define

TestInfoRequest::TestInfoRequest(id problemId) : Request(Api::TestInfoRequest) {
    this->set(constant.id, problemId);
}

/// endregion

#endif //JUDGE_TEST_INFO_REQUEST_H
