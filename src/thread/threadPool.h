//
// Created by keqing on 2021-10-10.
//

#ifndef JUDGE_THREADPOOL_H
#define JUDGE_THREADPOOL_H

#include "../util/__init__.h"
#include "job.h"

class ThreadPool {
private:
    Mutex<map<pthread_t, thread *>> threadPool;
    Mutex<queue<Job *>> enqueue;
    Mutex<vector<thread *>> deathThread;
    Mutex<int> needKill;
    condition_variable noTaskCv;
    mutex noTaskCvMutex;
    int maxCore;
    bool killed;

    Job *takeJob();

    virtual void addThread();

    void clean();

public:
    explicit ThreadPool(int core);

    void submit(Job *job);

    int getAccumulation();

    void updateCore(int newCount);

    void wait();

    void close();
};

/// region define

ThreadPool::ThreadPool(int core) : maxCore(core), killed(false), needKill(0) {
    for (int i = 0; i < core; ++i) addThread();
}

Job *ThreadPool::takeJob() {
    Job *cur = nullptr;
    enqueue.run([&](queue<Job *> &q) {
        if (q.empty()) return;
        cur = q.front();
        q.pop();
    });
    return cur;
}

void ThreadPool::addThread() {
    auto work = [&]() {
        while (true) {
            Job *cur = takeJob();
            if (cur != nullptr) {
                cur->exec();
                delete cur;
            } else {
                bool dead = false;
                needKill.run([&](int &count) {
                    if (count <= 0) return;
                    dead = true;
                    count--;
                });
                if (dead) break;

                clean();
                unique_lock<mutex> lk(noTaskCvMutex);
                noTaskCv.wait(lk);
            }
        }
        threadPool.run([&](map<pthread_t, thread *> &data) {
            auto iter = data.find(pthread_self());
            deathThread.run([&](vector<thread *> &data) {
                data.push_back(iter->second);
            });
            data.erase(iter);
        });
    };

    auto *newThread = new thread(work);
    threadPool.run([&](map<pthread_t, thread *> &data) {
        data.insert({newThread->native_handle(), newThread});
    });
}

void ThreadPool::clean() {
    if (deathThread.get().empty()) return;
    deathThread.run([&](vector<thread *> &data) {
        for (auto &item: data) delete item;
    });
}

void ThreadPool::submit(Job *job) {
    if (killed) return;
    enqueue.run([&](queue<Job *> &q) {
        q.push(job);
        noTaskCv.notify_one();
    });
}

int ThreadPool::getAccumulation() {
    return (int) enqueue.get().size();
}

void ThreadPool::updateCore(int newCount) {
    if (killed) return;
    needKill.run([&](int &cleaned) {
        if (newCount > this->maxCore)
            for (int i = this->maxCore; i < newCount; ++i)
                addThread();
        else {
            cleaned += this->maxCore - newCount;
            noTaskCv.notify_all();
        }
    });
}

void ThreadPool::wait() {
    updateCore(0);
    killed = true;
    map<pthread_t, thread *> tmp = threadPool.get();
    for (auto &item: tmp)
        item.second->join();
}

void ThreadPool::close() {
    killed = true;
    map<pthread_t, thread *> tmp = threadPool.get();
    for (auto &item: tmp) {
        pthread_kill(item.first, SIGKILL);
        delete item.second;
    }
}

/// endregion

#endif //JUDGE_THREADPOOL_H
