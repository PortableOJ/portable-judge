//
// Created by 胡柯青 on 2022-01-13.
//

#ifndef JUDGE_TEST_LIB_REQUEST_H
#define JUDGE_TEST_LIB_REQUEST_H

#include "../request.h"

class TestLibRequest : public Request {
public:
    TestLibRequest();
};

/// region define

TestLibRequest::TestLibRequest() : Request(Api::TestLibRequest) {}

/// endregion

#endif //JUDGE_TEST_LIB_REQUEST_H
