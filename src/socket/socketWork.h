//
// Created by keqing on 2021-10-22.
//

#ifndef JUDGE_SOCKET_WORK_H
#define JUDGE_SOCKET_WORK_H

#include "request.h"
#include "callback.h"
#include "../thread/__init__.h"

class SocketWork : public Job {
private:
    Request *request;
    Callback *callback;

protected:
    void work() override;

public:
    /**
     * 所有 request 和 callback 均不会被 delete
     * 请保证执行期间存活
     * @param req
     * @param cal
     * @param cm
     */
    SocketWork(Request *req, Callback *cal, CountMutex *cm = nullptr);

    ~SocketWork() override;

    [[nodiscard]] Request *getRequest() const;

    [[nodiscard]] Callback *getCallback() const;
};
/// region define

void SocketWork::work() {}

SocketWork::SocketWork(Request *req, Callback *cal, CountMutex *cm) : Job(cm), request(req), callback(cal) {}

SocketWork::~SocketWork() = default;

Request *SocketWork::getRequest() const {
    return request;
}

Callback *SocketWork::getCallback() const {
    return callback;
}

/// endregion

#endif //JUDGE_SOCKET_WORK_H
