//
// Created by 胡柯青 on 2022-01-18.
//

#ifndef JUDGE_TEST_REPORT_OUTPUT_REQUEST_H
#define JUDGE_TEST_REPORT_OUTPUT_REQUEST_H

#include "../request.h"

class TestReportOutputRequest : public Request {
public:
    TestReportOutputRequest(id problem, int pos, const string &value);
};

/// region define

TestReportOutputRequest::TestReportOutputRequest(id problemId, int pos, const string &value)
        : Request((Api::TestReportOutputRequest)) {
    this->set(constant.id, problemId);
    this->set(constant.pos, pos);
    this->setComplex(constant.value, value);
}

/// endregion

#endif //JUDGE_TEST_REPORT_OUTPUT_REQUEST_H
