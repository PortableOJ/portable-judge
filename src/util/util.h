//
// Created by 胡柯青 on 2021-11-14.
//

#ifndef JUDGE_UTIL_H
#define JUDGE_UTIL_H

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

#endif //JUDGE_UTIL_H
