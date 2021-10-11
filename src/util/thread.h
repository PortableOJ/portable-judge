//
// Created by keqing on 2021-10-10.
//

#ifndef JUDGE_THREAD_H
#define JUDGE_THREAD_H

#include "include.h"
#include "process.h"

class Thread {
private:
    static unordered_map<pthread_t, Thread *> *localThread;

    pthread_t pthread;
    string name;
    Thread *father;
    vector<Thread *> children;
    vector<Process *> processChildren;

public:
    static void init();

    static void close();

    static Thread *ctx();

    explicit Thread(function<void()>);

    Thread(void *(*)(void *));

    const string &getName() const;

    void start();

    void join();

    void kill();
};

unordered_map<pthread_t, Thread *> *Thread::localThread = nullptr;

void Thread::init() {
    if (localThread == nullptr) localThread = new unordered_map<pthread_t, Thread *>;
}

void Thread::close() {
    if (localThread == nullptr) return;
    delete localThread;
    localThread = nullptr;
}

Thread *Thread::ctx() {
    auto iter = localThread->find(pthread_self());
    if (iter == localThread->end()) return nullptr;
    return iter->second;
}

const string &Thread::getName() const {
    return name;
}

#endif //JUDGE_THREAD_H
