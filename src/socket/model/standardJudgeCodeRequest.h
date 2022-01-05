//
// Created by 胡柯青 on 2021-11-08.
//

#ifndef JUDGE_STANDARD_JUDGE_CODE_REQUEST_H
#define JUDGE_STANDARD_JUDGE_CODE_REQUEST_H

#include "../request.h"

class StandardJudgeCodeRequest : public Request {
public:
    explicit StandardJudgeCodeRequest(const string &name);
};

/// region define

StandardJudgeCodeRequest::StandardJudgeCodeRequest(const string &name) : Request(Api::StandardJudgeCodeRequest) {
    this->set(constant.name, name);
}

/// endregion

#endif //JUDGE_STANDARD_JUDGE_CODE_REQUEST_H
