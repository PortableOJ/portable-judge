//
// Created by 胡柯青 on 2022-01-18.
//

#ifndef JUDGE_TEST_RESULT_REPORT_REQUEST_H
#define JUDGE_TEST_RESULT_REPORT_REQUEST_H

#include "../request.h"

class TestCompileReportRequest : public Request {
public:
    explicit TestCompileReportRequest(id problemId);
};

/// region define

TestCompileReportRequest::TestCompileReportRequest(id problemId)
        : Request(Api::TestResultReportRequest) {
    this->set(constant.id, problemId);
}

/// endregion

#endif //JUDGE_TEST_RESULT_REPORT_REQUEST_H
