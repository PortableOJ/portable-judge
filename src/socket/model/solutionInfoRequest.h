//
// Created by 胡柯青 on 2021-11-14.
//

#ifndef JUDGE_SOLUTION_INFO_REQUEST_H
#define JUDGE_SOLUTION_INFO_REQUEST_H

#include "../request.h"

class SolutionInfoRequest : public Request {
public:
    explicit SolutionInfoRequest(id solutionId);
};

/// region define

SolutionInfoRequest::SolutionInfoRequest(id solutionId) : Request(Api::SolutionInfoRequest) {
    this->set(constant.id, solutionId);
}

/// endregion


#endif //JUDGE_SOLUTION_INFO_REQUEST_H
