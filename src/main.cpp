//
// Created by keqing on 2021-10-29.
//

#include "util/__init__.h"
#include "socket/__init__.h"
#include "file/__init__.h"

int main() {
    Logger::init();

    Logger::info("Loading env");
    Env::init("env");
    Env *env = Env::ctx();

    Session session(*env->getString(constant.serverUrl), env->getInt(constant.serverPort));
    RegisterRequest registerRequest;
    Callback callback(nullptr, [&](void *, stringstream &ss) {
        auto str = new string;
        ss >> *str;
        env->set(constant.judgeCode, str);
    });
    session.send(&registerRequest, &callback);
    session.close();

    Logger::info("This Judge Code: %", *env->getString(constant.judgeCode));

    Logger::info("Init socket and thread pool, thread: %, socket: %",
                 env->getInt(constant.initThreadCore),
                 env->getInt(constant.initSocketCore));
    auto *threadPool = new ThreadPool(env->getInt(constant.initThreadCore));
    auto socketPool = new SessionPool(env->getInt(constant.initSocketCore));
    threadPool->init();
    socketPool->init();
    FileManager fileManager;
    bool result = fileManager.init(socketPool, threadPool);
    if (!result) {
        Logger::err("init file manager fail");
        return -1;
    }

    Env::close();

    Logger::close();
}

/*
Register
0
5
abcde
0

Append
0
0

StandardJudge
0
4
main
0

StandardJudgeCode
0
93
#include <iostream>

using namespace std;

int main() {
    cout << "Hello world" << endl;
}
0
*/