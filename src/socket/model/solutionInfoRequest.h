//
// Created by keqing on 2021-11-14.
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
    this->set(constant.id, to_string(solutionId));
}

/// endregion


#endif //JUDGE_SOLUTION_INFO_REQUEST_H
