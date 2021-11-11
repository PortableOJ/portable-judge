#include "thread/__init__.h"
#include "socket/__init__.h"
#include "compiler/__init__.h"
#include "file/__init__.h"

int main() {
    Logger::init();

    Env::init(constant.envFileName);
    Env *env = Env::ctx();

    string code = "MyCode";
    env->set(constant.judgeCode, &code);

    auto *socketPool = new SessionPool(env->getInt(constant.initSocketCore));
    socketPool->init();

    RegisterRequest registerRequest;
    AppendRequest appendRequest;
    Callback callback1(string("test"));
    Callback callback2(string("test"));
    CountMutex cm(2);

    Job *job1 = new SocketWork(&registerRequest, &callback1, &cm);
    Job *job2 = new SocketWork(&appendRequest, &callback2, &cm);
    socketPool->submit(job1);
    socketPool->submit(job2);

    cm.wait();

    socketPool->wait();
    delete socketPool;

    Logger::close();
}
