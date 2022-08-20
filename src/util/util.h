//
// Created by 胡柯青 on 2021-11-14.
//

#ifndef JUDGE_UTIL_H
#define JUDGE_UTIL_H

#include "include.h"

int toInt(const string &str) {
    int tmp = 0;
    bool flag = false;
    if (str[0] == '-') flag = true;
    for (int i = flag ? 1 : 0; i < str.size(); ++i) {
        if (str[i] < '0' || str[i] > '9') return false;
        tmp *= 10;
        tmp += str[i] - '0';
    }
    return tmp * (flag ? -1 : 1);
}

unsigned long toUL(const string &str) {
    int tmp = 0;
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] < '0' || str[i] > '9') return false;
        tmp *= 10;
        tmp += str[i] - '0';
    }
    return tmp;
}

int readInt(int socketId) {
    char c;
    int code = 0;
    while (read(socketId, &c, 1)) {
        if (isdigit(c)) {
            code *= 10;
            code += c - '0';
        } else break;
    }
    return code;
}

#endif //JUDGE_UTIL_H
