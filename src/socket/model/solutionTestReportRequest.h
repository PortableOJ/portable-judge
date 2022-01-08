//
// Created by 胡柯青 on 2021-11-14.
//

#ifndef JUDGE_SOLUTION_REPORT_REQUEST_H
#define JUDGE_SOLUTION_REPORT_REQUEST_H

#include "../request.h"

class SolutionTestReportRequest : public Request {
public:
    SolutionTestReportRequest(const string &value, id solutionId, const string &msg, int timeCost, int memoryCost);
};

/// region define

SolutionTestReportRequest::SolutionTestReportRequest(
        const string &value, id solutionId, const string &msg, int timeCost, int memoryCost)
        : Request(Api::SolutionTestReportRequest) {
    this->set(constant.value, value);
    this->set(constant.id, solutionId);
    this->set(constant.msg, msg);

}

/// endregion

#endif //JUDGE_SOLUTION_REPORT_REQUEST_H
