//
// Created by 胡柯青 on 2022-01-22.
//

#ifndef JUDGE_TEST_REPORT_OVER_H
#define JUDGE_TEST_REPORT_OVER_H

#include "../request.h"

class TestReportOver : public Request {
public:
    explicit TestReportOver(id problemId);
};

/// region define

TestReportOver::TestReportOver(id problemId) : Request(Api::TestReportOverRequest) {
    this->set(constant.id, problemId);
}

/// endregion

#endif //JUDGE_TEST_REPORT_OVER_H
