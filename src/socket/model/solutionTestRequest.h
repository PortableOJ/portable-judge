//
// Created by 胡柯青 on 2022-01-07.
//

#ifndef JUDGE_SOLUTION_TEST_H
#define JUDGE_SOLUTION_TEST_H

#include "../request.h"

class SolutionTestRequest : public Request {
public:
    explicit SolutionTestRequest(id solutionId);
};

/// region define

SolutionTestRequest::SolutionTestRequest(id solutionId) : Request(Api::SolutionTestRequest) {
    this->set(constant.id, solutionId);
}

/// endregion

#endif //JUDGE_SOLUTION_TEST_H
