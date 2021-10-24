#include "thread/__init__.h"
#include "socket/__init__.h"

int main() {
    /// region logger

    Logger::init();

    /// region env

    Env::init(constant.envFileName);
    Env *env = Env::ctx();

    /// region thread pool

    auto *pool = new ThreadPool(env->getInt(constant.initThreadCore));
    pool->init();
    env->set(constant.workThreadPool, pool);

    /// region session pool

    string code = "MyCode";
    env->set(constant.judgeCode, &code);
    auto *socketPool = new SessionPool(env->getInt(constant.initSocketCore));
    socketPool->init();
    env->set(constant.workSocketPool, socketPool);

    /// region work

    CountMutex cm(100);

    for (long i = 0; i < 100; ++i) {
        Job *job = new Task((void *) i, [](void *data) {
            Logger::info("%", (long) data);
        }, &cm);
        pool->submit(job);
    }

    cm.wait();

    cm.reset(2);

    RegisterRequest registerRequest;
    AppendRequest appendRequest;
    Callback callback(nullptr, [&](void *, stringstream &ss) {
        string ans;
        getline(ss, ans);
        Logger::trace("%", ans);
    });
    Job *job1 = new SocketWork(&registerRequest, &callback, &cm);
    Job *job2 = new SocketWork(&appendRequest, &callback, &cm);
    socketPool->submit(job1);
    socketPool->submit(job2);

    cm.wait();

    /// endregion

    socketPool->wait();
    delete socketPool;

    /// endregion


    pool->wait();
    delete pool;

    /// endregion

    Env::close();

    /// endregion

    Logger::close();

    /// endregion
}
