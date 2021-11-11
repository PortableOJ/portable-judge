//
// Created by keqing on 2021-11-10.
//

#ifndef JUDGE_SOLUTION_CODE_REQUEST_H
#define JUDGE_SOLUTION_CODE_REQUEST_H

#include "../request.h"

class SolutionCodeRequest : public Request {
public:
    explicit SolutionCodeRequest(int solutionId);
};

/// region define

SolutionCodeRequest::SolutionCodeRequest(int solutionId) : Request(Api::SolutionCodeRequest) {
    this->set(constant.id, to_string(solutionId));
}

/// endregion

#endif //JUDGE_SOLUTION_CODE_REQUEST_H
