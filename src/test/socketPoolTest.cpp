//
// Created by keqing on 2021-10-29.
//

#include "../socket/__init__.h"

int main() {
    Logger::init();

    Env::init("env");
    Env *env = Env::ctx();
    string code = "MyCode";
    env->set(constant.judgeCode, &code);
    auto *socketPool = new SessionPool(env->getInt(constant.initSocketCore));
    socketPool->init();

    RegisterRequest registerRequest;
    AppendRequest appendRequest;
    Callback callback(nullptr, [&](void *, stringstream &ss) {
        string ans;
        getline(ss, ans);
        Logger::trace("%", ans);
    });
    CountMutex cm(100);

    Job *job1 = new SocketWork(&registerRequest, &callback, &cm);
    Job *job2 = new SocketWork(&appendRequest, &callback, &cm);
    socketPool->submit(job1);
    socketPool->submit(job2);

    cm.wait();

    socketPool->wait();
    delete socketPool;

    Logger::close();
}

