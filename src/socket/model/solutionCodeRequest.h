//
// Created by 胡柯青 on 2021-11-10.
//

#ifndef JUDGE_SOLUTION_CODE_REQUEST_H
#define JUDGE_SOLUTION_CODE_REQUEST_H

#include "../request.h"

class SolutionCodeRequest : public Request {
public:
    explicit SolutionCodeRequest(id solutionId);
};

/// region define

SolutionCodeRequest::SolutionCodeRequest(id solutionId) : Request(Api::SolutionCodeRequest) {
    this->set(constant.id, solutionId);
}

/// endregion

#endif //JUDGE_SOLUTION_CODE_REQUEST_H
