//
// Created by 胡柯青 on 2022-01-18.
//

#ifndef JUDGE_TEST_REPORT_OUTPUT_REQUEST_H
#define JUDGE_TEST_REPORT_OUTPUT_REQUEST_H

#include "../request.h"

class TestReportOutputRequest : public Request {
public:
    explicit TestReportOutputRequest(id problem);

    void setFlag(bool flag);

    void setTestName(const string &name);

    void setPos(int pos);

    void setValue(const string &value);
};

/// region define

TestReportOutputRequest::TestReportOutputRequest(id problemId)
        : Request((Api::TestReportOutputRequest)) {
    this->set(constant.id, problemId);
}

void TestReportOutputRequest::setFlag(bool flag) {
    this->set(constant.flag, flag);
}

void TestReportOutputRequest::setTestName(const string &name) {
    this->set(constant.name, name);
}

void TestReportOutputRequest::setPos(int pos) {
    this->set(constant.pos, pos);
}

void TestReportOutputRequest::setValue(const string &value) {
    this->setComplex(constant.value, value);
}

/// endregion

#endif //JUDGE_TEST_REPORT_OUTPUT_REQUEST_H
