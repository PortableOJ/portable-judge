//
// Created by 胡柯青 on 2021-11-11.
//

#ifndef JUDGE_PROBLEM_DATA_IN_REQUEST_H
#define JUDGE_PROBLEM_DATA_IN_REQUEST_H

#include "../request.h"

class ProblemDataInRequest : public Request {
public:
    ProblemDataInRequest(id problemId, const string &inputName);
};

/// region define

ProblemDataInRequest::ProblemDataInRequest(id problemId, const string &inputName) : Request(Api::ProblemDataInRequest) {
    this->set(constant.id, problemId);
    this->set(constant.name, inputName);
}

// endregion

#endif //JUDGE_PROBLEM_DATA_IN_REQUEST_H
