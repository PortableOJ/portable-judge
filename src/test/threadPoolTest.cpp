//
// Created by 胡柯青 on 2021-10-29.
//

#include "../thread/__init__.h"

int main() {
    Logger::init();

    auto *pool = new ThreadPool(env->getInt(constant.initThreadCore));
    pool->init();

    CountMutex cm(100);

    for (long i = 0; i < 100; ++i) {
        Job *job = new Task((void *) i, [](void *data) {
            Logger::info("%", (long) data);
        }, &cm);
        pool->submit(job);
    }

    cm.wait();

    pool->wait();
    delete pool;

    Logger::close();
}

