//
// Created by 胡柯青 on 2021-11-08.
//

#ifndef JUDGE_STANDARD_JUDGE_REQUEST_H
#define JUDGE_STANDARD_JUDGE_REQUEST_H

#include "../request.h"

class StandardJudgeRequest : public Request {
public:
    StandardJudgeRequest();
};

StandardJudgeRequest::StandardJudgeRequest() : Request(Api::StandardJudgeRequest) {}

#endif //JUDGE_STANDARD_JUDGE_REQUEST_H
