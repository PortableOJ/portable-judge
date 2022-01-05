//
// Created by 胡柯青 on 2021-11-14.
//

#ifndef JUDGE_SOLUTION_REPORT_REQUEST_H
#define JUDGE_SOLUTION_REPORT_REQUEST_H

#include "../request.h"

class SolutionReportRequest : public Request {
public:
    SolutionReportRequest(const string &type, const string &value);

    void setSolutionId(id solutionId);

    void setLimitResult(unsigned long timeCost, unsigned long memoryCost);
};

/// region define

SolutionReportRequest::SolutionReportRequest(const string &type, const string &value) : Request(Api::SolutionReportRequest) {
    this->set(constant.type, type);
    this->set(constant.value, value);
}

void SolutionReportRequest::setSolutionId(id solutionId) {
    this->set(constant.id, to_string(solutionId));
}

void SolutionReportRequest::setLimitResult(unsigned long timeCost, unsigned long memoryCost) {
    this->set(constant.timeCost, to_string(timeCost));
    this->set(constant.memoryCost, to_string(memoryCost));
}

/// endregion

#endif //JUDGE_SOLUTION_REPORT_REQUEST_H
