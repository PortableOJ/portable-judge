//
// Created by keqing on 2021-11-11.
//

#ifndef JUDGE_PROBLEM_DATA_IN_REQUEST_H
#define JUDGE_PROBLEM_DATA_IN_REQUEST_H

#include "../request.h"

class ProblemDataInRequest : public Request {
public:
    ProblemDataInRequest(int problemId, const string &inputName);
};

/// region define

ProblemDataInRequest::ProblemDataInRequest(int problemId, const string &inputName) : Request(Api::ProblemDataInRequest) {
    this->set(constant.id, to_string(problemId));
    this->set(constant.name, inputName);
}

// endregion

#endif //JUDGE_PROBLEM_DATA_IN_REQUEST_H
