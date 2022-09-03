//
// Created by 胡柯青 on 2022-01-05.
//

#ifndef JUDGE_HEARTBEAT_REQUEST_H
#define JUDGE_HEARTBEAT_REQUEST_H

#include "../request.h"

class HeartbeatRequest : public Request {
public:
    HeartbeatRequest();

    void updateSocket(int newValue);
    void updateWork(int newValue);
};

/// region define

HeartbeatRequest::HeartbeatRequest() : Request(Api::Heartbeat) {
}

void HeartbeatRequest::updateSocket(int newValue) {
    this->set(constant.socketAccumulation, newValue);
}

void HeartbeatRequest::updateWork(int newValue) {
    this->set(constant.workAccumulation, newValue);
}
/// endregion

#endif //JUDGE_HEARTBEAT_REQUEST_H
