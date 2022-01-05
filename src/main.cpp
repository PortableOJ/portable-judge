//
// Created by 胡柯青 on 2021-10-29.
//

#include "util/__init__.h"
#include "socket/__init__.h"
#include "file/__init__.h"
#include "work/__init__.h"

int main() {
    Logger::init();

    Logger::info("Loading env");
    Env::init("env");
    Env *env = Env::ctx();

    Session session(*env->getString(constant.serverUrl), env->getInt(constant.serverPort));
    RegisterRequest registerRequest;
    Callback registerCallback(nullptr, [&](void *, stringstream &ss) {
        auto str = new string;
        ss >> *str;
        env->set(constant.judgeCode, str);
    });
    session.send(&registerRequest, &registerCallback);
    session.close();

    Logger::info("This Judge Code: %", *env->getString(constant.judgeCode));

    Logger::info("Init socket and thread pool, thread: %, socket: %",
                 env->getInt(constant.initThreadCore),
                 env->getInt(constant.initSocketCore));
    auto *threadPool = new ThreadPool(env->getInt(constant.initThreadCore));
    auto *workPool = new ThreadPool(env->getInt(constant.initWorkCore));
    auto socketPool = new SessionPool(env->getInt(constant.initSocketCore));
    threadPool->init();
    socketPool->init();
    FileManager fileManager;
    bool fileManagerResult = fileManager.init(socketPool, threadPool);
    if (!fileManagerResult) {
        Logger::err("init file manager fail");
        return -1;
    }

    bool terminate = false;
    int heartbeatTime = env->getInt(constant.heartbeatTime);
    HeartbeatRequest heartbeatRequest(threadPool->getAccumulation(), socketPool->getAccumulation());
    Callback heartbeatCallback(nullptr, [&](void *, stringstream &ss) {
        string key;
        id value;
        while (ss >> key >> value) {
            if (key == constant.terminate) {
                terminate = true;
            } else if (key == constant.judgeTask) {
                auto task = new Task((void *) value, [&](void *data) {
                    JudgeWork judgeWork((id) value, threadPool, socketPool);
                    judgeWork.start();
                });
                workPool->submit(task);
            } else if (key == constant.testTask) {
                auto task = new Task((void *) value, [&](void *data) {
                    TestWork testWork((id) value, threadPool, socketPool);
                    testWork.start();
                });
                workPool->submit(task);
            } else if (key == constant.threadCore) {
                threadPool->updateCore((int) value);
            } else if (key == constant.socketCore) {
                socketPool->updateCore((int) value);
            } else if (key == constant.workCore) {
                workPool->updateCore((int) value);
            }
        }
    });
    CountMutex heartbeatCM(1);

    while (!terminate) {
        heartbeatCM.reset(1);
        auto *socketWork = new SocketWork(&heartbeatRequest, &heartbeatCallback, &heartbeatCM);
        socketPool->submit(socketWork);
        heartbeatCM.wait();
        sleep(heartbeatTime);
    }

    workPool->close();
    socketPool->close();
    threadPool->close();

    delete workPool;
    delete socketPool;
    delete threadPool;

    auto judgeCode = env->getString(constant.judgeCode);
    delete judgeCode;

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