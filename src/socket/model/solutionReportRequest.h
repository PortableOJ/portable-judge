//
// Created by keqing on 2021-11-14.
//

#ifndef JUDGE_SOLUTION_REPORT_REQUEST_H
#define JUDGE_SOLUTION_REPORT_REQUEST_H

#include "../request.h"

class SolutionReportRequest : public Request {
public:
    SolutionReportRequest(const string &type, const string &value);
};

/// region define

SolutionReportRequest::SolutionReportRequest(const string &type, const string &value) : Request(Api::SolutionReportRequest) {
    this->set(constant.type, type);
    this->set(constant.value, value);
}

/// endregion

#endif //JUDGE_SOLUTION_REPORT_REQUEST_H
