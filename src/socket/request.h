//
// Created by 胡柯青 on 2021-10-16.
//

#ifndef JUDGE_REQUEST_H
#define JUDGE_REQUEST_H

#include "../util/__init__.h"
#include "../env/__init__.h"

namespace Api {
    const char *Register = "Register";

    const char *Append = "Append";
    const char *Heartbeat = "Heartbeat";
    const char *SolutionCompileMsgReportRequest = "SolutionCompileMsgReport";
    const char *SolutionInfoRequest = "SolutionInfo";
    const char *SolutionTestRequest = "SolutionTest";
    const char *SolutionTestReportRequest = "SolutionTestReport";
    const char *SolutionCodeRequest = "SolutionCode";
    const char *StandardJudgeRequest = "StandardJudge";
    const char *StandardJudgeCodeRequest = "StandardJudgeCode";
    const char *TestInfoRequest = "TestInfo";
    const char *TestStdCodeRequest = "TestStdCode";
    const char *TestReportOutputRequest = "TestReportOutput";
    const char *TestResultReportRequest = "TestResultReport";
    const char *TestTestRequest = "TestTest";
    const char *TestReportOverRequest = "TestReportOver";
    const char *TestLibRequest = "TestLibRequest";
    const char *ProblemJudgeCodeRequest = "ProblemJudgeCode";
    const char *ProblemDataInRequest = "ProblemDataIn";
    const char *ProblemDataOutRequest = "ProblemDataOut";
}

class Request {
private:
    const char *method;
    map<string, string> data;
    map<string, string> complexData;

protected:
    explicit Request(const char *m);

    void set(const string &key, const string &value);

    void set(const string &key, int value);

    void set(const string &key, id value);

    void set(const string &key, bool value);

    void setComplex(const string &key, const string &value);

public:

    void send(int socketId);

    [[nodiscard]] const char *getMethod() const;
};

/// region define

Request::Request(const char *m) : method(m) {}

void Request::set(const string &key, const string &value) {
    data.insert({key, value});
}

void Request::set(const string &key, int value) {
    data.insert({key, to_string(value)});
}

void Request::set(const string &key, id value) {
    data.insert({key, to_string(value)});
}

void Request::set(const string &key, bool value) {
    if (value) {
        data.insert({key, "true"});
    } else {
        data.insert({key, "false"});
    }
}

void Request::setComplex(const string &key, const string &value) {
    complexData.insert({key, value});
}

void Request::send(int socketId) {
    int bufferLen = 0;
    for (auto &item: data)
        bufferLen += (int) item.first.length() + (int) item.second.length() + 2;
    char *tmp = new char[100];
    int headLen = sprintf(tmp, "%s\n%d\n", method, bufferLen);
    write(socketId, tmp, headLen);
    delete[] tmp;
    for (auto &item: data) {
        write(socketId, item.first.c_str(), item.first.length());
        write(socketId, " ", 1);
        write(socketId, item.second.c_str(), item.second.length());
        write(socketId, "\n", 1);
    }
    for (auto &item: complexData) {
        string len = to_string(item.second.length());
        write(socketId, item.first.c_str(), item.first.length());
        write(socketId, " ", 1);
        write(socketId, len.c_str(), len.length());
        write(socketId, "\n", 1);
        write(socketId, item.second.c_str(), item.second.length());
    }
    if (!data.empty()) {
        write(socketId, "0\n", 2);
    }
}

const char *Request::getMethod() const {
    return method;
}

/**
 * REQUEST Format
 *
 * ${METHOD}
 * ${len}
 * abc
 * ${len}
 * abc
 * 0
 *
 * RESPONSE Format
 * ${code}
 * ${len}
 * abc
 * ${len}
 * abc
 * 0
 */

/// endregion

#endif //JUDGE_REQUEST_H
