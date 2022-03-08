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

    Runner::init();
    Compiler::init();

    FileManager::setHome();

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
    workPool->init();
    socketPool->init();
    FileManager fileManager;
    bool fileManagerResult = fileManager.init(socketPool, threadPool);
    if (!fileManagerResult) {
        Logger::err("init file manager fail");
        return -1;
    }

    bool terminate = false;
    int heartbeatTime = env->getInt(constant.heartbeatTime);
    HeartbeatRequest heartbeatRequest;
    Callback heartbeatCallback(nullptr, [&](void *, stringstream &ss) {
        string key;
        id value;
        while (ss >> key >> value) {
            if (key == constant.terminate) {
                terminate = true;
            } else if (key == constant.judgeTask) {
                Logger::trace("add judge job %", value);
                auto task = new Task((void *) value, [&](void *data) {
                    JudgeWork judgeWork((id) data, threadPool, socketPool);
                    judgeWork.start();
                });
                workPool->submit(task);
            } else if (key == constant.testTask) {
                Logger::trace("add test job %", value);
                auto task = new Task((void *) value, [&](void *data) {
                    TestWork testWork((id) data, threadPool, socketPool);
                    testWork.start();
                });
                workPool->submit(task);
            } else if (key == constant.threadCore) {
                threadPool->updateCore((int) value);
            } else if (key == constant.socketCore) {
                socketPool->updateCore((int) value);
            } else if (key == constant.workCore) {
                workPool->updateCore((int) value);
            } else if (key == constant.cleanProblem) {
                FileManager::cleanProblem(value);
            }
        }
    });
    CountMutex heartbeatCM(1);
    SocketWork socketWork(&heartbeatRequest, &heartbeatCallback, &heartbeatCM, true);

    while (!terminate) {
        heartbeatCM.reset(1);
        heartbeatRequest.updateThread(threadPool->getAccumulation());
        heartbeatRequest.updateSocket(socketPool->getAccumulation());
        heartbeatRequest.updateWork(workPool->getAccumulation());
        socketPool->submit(&socketWork);
        heartbeatCM.wait();
        sleep(heartbeatTime);
    }

    workPool->wait();
    threadPool->wait();
    socketPool->wait();

    delete workPool;
    delete socketPool;
    delete threadPool;

    auto judgeCode = env->getString(constant.judgeCode);
    delete judgeCode;

    Env::close();

    Logger::close();
}
