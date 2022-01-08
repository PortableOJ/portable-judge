//
// Created by 胡柯青 on 2022-01-07.
//

#ifndef JUDGE_SOLUTION_COMPILE_MSG_REPORT_REQUEST_H
#define JUDGE_SOLUTION_COMPILE_MSG_REPORT_REQUEST_H

#include "../request.h"

class SolutionCompileMsgReportRequest : public Request {
public:
    SolutionCompileMsgReportRequest(id solutionId, bool value, bool judge, const string &msg);
};

/// region define

SolutionCompileMsgReportRequest::SolutionCompileMsgReportRequest(id solutionId, bool value, bool judge, const string &msg)
        : Request(Api::SolutionCompileMsgReportRequest) {
    this->set(constant.id, solutionId);
    this->set(constant.value, value);
    this->set(constant.judge, judge);
    this->setComplex(constant.msg, msg);
}

/// endregion

#endif //JUDGE_SOLUTION_COMPILE_MSG_REPORT_REQUEST_H
