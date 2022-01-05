//
// Created by 胡柯青 on 2021-10-12.
//

#ifndef JUDGE_MUTEX_H
#define JUDGE_MUTEX_H

template<class T>
class Mutex {
    mutex m;
    T data;
public:
    Mutex() = default;

    explicit Mutex(const T &rhs) : data(rhs) {}

    void run(const function<void(T &)> &func) {
        lock_guard<mutex> lockGuard(m);
        func(data);
    }

    const T &get() const {
        return data;
    }

    void set(const T &rhs) {
        lock_guard<mutex> lockGuard(m);
        data = rhs;
    }
};

#endif //JUDGE_MUTEX_H
