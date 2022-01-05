//
// Created by 胡柯青 on 2021-11-11.
//

#ifndef JUDGE_PROBLEM_JUDGE_CODE_REQUEST_H
#define JUDGE_PROBLEM_JUDGE_CODE_REQUEST_H

#include "../request.h"

class ProblemJudgeCodeRequest : public Request {
public:
    explicit ProblemJudgeCodeRequest(int id);
};

/// region define

ProblemJudgeCodeRequest::ProblemJudgeCodeRequest(int id) : Request(Api::ProblemJudgeCodeRequest) {
    this->set(constant.id, to_string(id));
}

/// endregion

#endif //JUDGE_PROBLEM_JUDGE_CODE_REQUEST_H
