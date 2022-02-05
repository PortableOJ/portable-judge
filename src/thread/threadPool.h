//
// Created by 胡柯青 on 2021-10-10.
//

#ifndef JUDGE_THREAD_POOL_H
#define JUDGE_THREAD_POOL_H

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

    void addThread();

    void clean();

protected:
    Job *takeJob();

    virtual function<void()> workFunc();

    void killSelf();

    bool sleep();

public:
    explicit ThreadPool(int core);

    void init();

    /**
     * 提交任务，任务会被 delete
     * @param job 任务指针
     */
    void submit(Job *job);

    [[nodiscard]] int getAccumulation() const;

    [[nodiscard]] int getMaxCore() const;

    void updateCore(int newCount);

    void wait();

    void close();
};

/// region define

void ThreadPool::addThread() {
    auto *newThread = new thread(workFunc());
    threadPool.run([&](map<pthread_t, thread *> &data) {
        data.insert({newThread->native_handle(), newThread});
    });
}

void ThreadPool::clean() {
    if (deathThread.get().empty()) return;
    deathThread.run([&](vector<thread *> &data) {
        for (auto &item: data) {
            item->join();
            delete item;
        }
    });
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

function<void()> ThreadPool::workFunc() {
    return [&]() {
        while (true) {
            Job *cur = takeJob();
            if (cur != nullptr) {
                cur->exec();
                if (cur->toDelete()) {
                    delete cur;
                }
            } else {
                if (sleep()) {
                    break;
                }
            }
        }
        killSelf();
    };

}

void ThreadPool::killSelf() {
    threadPool.run([&](map<pthread_t, thread *> &data) {
        auto iter = data.find(pthread_self());
        deathThread.run([&](vector<thread *> &data) {
            data.push_back(iter->second);
        });
        data.erase(iter);
    });
}

bool ThreadPool::sleep() {
    bool dead = false;
    needKill.run([&](int &count) {
        if (count <= 0) return;
        dead = true;
        count--;
    });
    if (dead) return true;

    clean();
    unique_lock<mutex> lk(noTaskCvMutex);
    noTaskCv.wait(lk);
    return false;
}

ThreadPool::ThreadPool(int core) : maxCore(core), killed(false), needKill(0) {}

void ThreadPool::init() {
    for (int i = 0; i < maxCore; ++i) addThread();
}

void ThreadPool::submit(Job *job) {
    if (killed) return;
    enqueue.run([&](queue<Job *> &q) {
        q.push(job);
        noTaskCv.notify_one();
    });
}

int ThreadPool::getAccumulation() const {
    return (int) enqueue.get().size();
}

int ThreadPool::getMaxCore() const {
    return maxCore;
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

#endif //JUDGE_THREAD_POOL_H
