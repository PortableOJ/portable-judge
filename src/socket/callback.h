//
// Created by keqing on 2021-10-16.
//

#ifndef JUDGE_CALLBACK_H
#define JUDGE_CALLBACK_H

#include "../util/__init__.h"

const int bufferSize = 1024;

class Callback {
private:
    void *data;
    string *filePath;
    function<void(void *, stringstream &)> func;

    char *buffer;
    int dataLen;
    int socketId;

    char getNext();

    int nextLen();

    bool checkStatus();

    void reduceStream(stringstream &ss);

    void writeToFile();

    void clearSocket();

public:
    Callback(void *data, function<void(void *, stringstream &)> func);

    explicit Callback(const string &fileName);

    explicit Callback(const path &fileName);

    bool exec(int sid);
};

// region define

char Callback::getNext() {
    static int curPos = 0;
    static int curLen = 0;
    if (curPos >= curLen) {
        curLen = read(socketId, buffer, bufferSize);
        curPos = 0;
    }
    return buffer[curPos++];
}

int Callback::nextLen() {
    int res = 0;
    char c = getNext();
    while (!isdigit(c)) c = getNext();
    while (isdigit(c)) {
        res *= 10;
        res += c - '0';
        c = getNext();
    }
    return res;
}

bool Callback::checkStatus() {
    int code = nextLen();
    if (code != 0) {
        Logger::err("socket response code: %", code);
        return false;
    }
    return true;
}

void Callback::reduceStream(stringstream &ss) {
    for (int n = nextLen(); n > 0; n = nextLen())
        for (int i = 0; i < n; ++i)
            ss << getNext();
}

void Callback::writeToFile() {
    ofstream out(*filePath);
    for (int n = nextLen(); n > 0; n = nextLen())
        for (int i = 0; i < n; ++i)
            out << getNext();
}

void Callback::clearSocket() {
    for (int n = nextLen(); n > 0; n = nextLen())
        for (int i = 0; i < n; ++i)
            getNext();
}

Callback::Callback(void *data, function<void(void *, stringstream &)> func)
        : data(data), filePath(nullptr), func(move(func)), buffer(nullptr), dataLen(0), socketId(-1) {}

Callback::Callback(const string &fileName)
        : data(nullptr), filePath(new string(fileName)), func(nullptr), buffer(nullptr), dataLen(0), socketId(-1) {}

Callback::Callback(const path &fileName)
        : data(nullptr), filePath(new string(fileName.string())), func(nullptr), buffer(nullptr), dataLen(0),
          socketId(-1) {}

bool Callback::exec(int sid) {
    this->socketId = sid;
    buffer = new char[bufferSize];
    memset(buffer, 0, sizeof(char) * bufferSize);
    if (!checkStatus()) {
        clearSocket();
        delete[] buffer;
        return false;
    } else if (filePath == nullptr) {
        stringstream ss;
        reduceStream(ss);
        delete[] buffer;
        func(data, ss);
        return true;
    } else {
        writeToFile();
        delete[] buffer;
//        delete filePath;
//        filePath = nullptr;
        return true;
    }
}

// endregion

#endif //JUDGE_CALLBACK_H
