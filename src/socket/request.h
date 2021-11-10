//
// Created by keqing on 2021-10-16.
//

#ifndef JUDGE_REQUEST_H
#define JUDGE_REQUEST_H

#include "../util/__init__.h"
#include "../env/__init__.h"

namespace Api {
    const char *Register = "REGISTER";
    const char *Append = "APPEND";
    const char *Heartbeat = "HEARTBEAT";
    const char *Close = "CLOSE";
}

class Request {
private:
    const char *method;
    map<string, string> data;

protected:
    explicit Request(const char *m);

    void set(const string &key, const string &value);

public:

    void send(int socketId);
};

/// region define

Request::Request(const char *m) : method(m) {}

void Request::set(const string &key, const string &value) {
    data.insert({key, value});
}

void Request::send(int socketId) {
    int bufferLen = 0;
    for (auto &item : data)
        bufferLen += (int) item.first.length() + (int) item.second.length() + 2;
    char *tmp = new char[100];
    int headLen = sprintf(tmp, "BEGIN\n%s\n%d\n", method, bufferLen);
    write(socketId, tmp, headLen);
    delete[] tmp;
    for (auto &item : data) {
        write(socketId, item.first.c_str(), item.first.length());
        write(socketId, " ", 1);
        write(socketId, item.second.c_str(), item.second.length());
        write(socketId, "\n", 1);
    }
    if (!data.empty()) {
        write(socketId, "0\n", 2);
    }
    write(socketId, "END\n", 4);
}

/**
 * REQUEST Format
 *
 * BEGIN
 * ${METHOD}
 * ${len}
 * abc
 * ${len}
 * abc
 * 0
 * END
 *
 * RESPONSE Format
 * ${len}
 * abc
 * ${len}
 * abc
 * 0
 */

/// endregion

#endif //JUDGE_REQUEST_H