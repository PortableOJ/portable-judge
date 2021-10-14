#include "thread/threadPool.h"

int main() {
    /// region logger

    Logger::init();

    /// region env

    Env::init(constant.envFileName);
    Env *env = Env::ctx();

    /// region thread pool

    auto *pool = new ThreadPool(env->getInt(constant.maxThreadCore));
    env->set(constant.workThreadPool, pool);

    for (long i = 0; i < 100; ++i) {
        Job *job = new Job((void *) i, [](void *data) {
            Logger::info("%", (long) data);
        });
        pool->submit(job);
    }

    pool->wait();
    delete pool;

    /// endregion

    Env::close();

    /// endregion

    Logger::close();

    /// endregion
}
