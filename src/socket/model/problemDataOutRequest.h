//
// Created by 胡柯青 on 2021-11-11.
//

#ifndef JUDGE_PROBLEM_DATA_OUT_REQUEST_H
#define JUDGE_PROBLEM_DATA_OUT_REQUEST_H

#include "../request.h"

class ProblemDataOutRequest : public Request {
public:
    ProblemDataOutRequest(id problemId, const string &inputName);
};

/// region define

ProblemDataOutRequest::ProblemDataOutRequest(id problemId, const string &inputName) : Request(Api::ProblemDataOutRequest) {
    this->set(constant.id, problemId);
    this->set(constant.name, inputName);
}

// endregion

#endif //JUDGE_PROBLEM_DATA_OUT_REQUEST_H
