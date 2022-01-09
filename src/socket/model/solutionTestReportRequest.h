//
// Created by 胡柯青 on 2021-11-14.
//

#ifndef JUDGE_SOLUTION_REPORT_REQUEST_H
#define JUDGE_SOLUTION_REPORT_REQUEST_H

#include "../request.h"

class SolutionTestReportRequest : public Request {
public:

    SolutionTestReportRequest();

    SolutionTestReportRequest(int value, id solutionId, const string &msg,
                              unsigned long timeCost, unsigned long memoryCost);

    void setValue(int value);

    void setSolutionId(id solutionId);

    void setMsg(const string &msg);

    void setTimeCost(unsigned long timeCost);

    void setMemoryCost(unsigned long memoryCost);
};

/// region define

SolutionTestReportRequest::SolutionTestReportRequest() : Request(Api::SolutionTestReportRequest) {}

SolutionTestReportRequest::SolutionTestReportRequest(
        int value, id solutionId, const string &msg, unsigned long timeCost, unsigned long memoryCost)
        : Request(Api::SolutionTestReportRequest) {
    this->set(constant.value, value);
    this->set(constant.id, solutionId);
    this->setComplex(constant.msg, msg);
    this->set(constant.timeCost, timeCost);
    this->set(constant.memoryCost, memoryCost);
}

void SolutionTestReportRequest::setValue(int value) {
    this->set(constant.value, value);
}

void SolutionTestReportRequest::setSolutionId(id solutionId) {
    this->set(constant.id, solutionId);
}

void SolutionTestReportRequest::setMsg(const string &msg) {
    this->setComplex(constant.msg, msg);
}

void SolutionTestReportRequest::setTimeCost(unsigned long timeCost) {
    this->set(constant.timeCost, timeCost);
}

void SolutionTestReportRequest::setMemoryCost(unsigned long memoryCost) {
    this->set(constant.memoryCost, memoryCost);
}

/// endregion

#endif //JUDGE_SOLUTION_REPORT_REQUEST_H