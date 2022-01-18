//
// Created by 胡柯青 on 2022-01-18.
//

#ifndef JUDGE_TEST_RESULT_REPORT_REQUEST_H
#define JUDGE_TEST_RESULT_REPORT_REQUEST_H

#include "../request.h"

class TestResultReportRequest : public Request {
public:
    TestResultReportRequest(id problemId, bool result);
};

/// region define

TestResultReportRequest::TestResultReportRequest(id problemId, bool result)
        : Request(Api::TestResultReportRequest) {
    this->set(constant.id, problemId);
    this->set(constant.value, result);
}

/// endregion

#endif //JUDGE_TEST_RESULT_REPORT_REQUEST_H
