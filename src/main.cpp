#include "thread/threadPool.h"

int main() {
    Logger::init();
    ThreadPool::ctx()->init(10);

    ThreadPool *pool = ThreadPool::ctx();
    for (int i = 0; i < 100; ++i) {
        Job *job = new Job((void *) i, [](void *data) {
            Logger::info("%", (long) data);
        });
        pool->submit(job);
    }

    ThreadPool::ctx()->wait();
    ThreadPool::ctx()->close();
    Logger::debug("%", ThreadPool::ctx()->getAccumulation());
    Logger::close();
}
