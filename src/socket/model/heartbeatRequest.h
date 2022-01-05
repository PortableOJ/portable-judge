//
// Created by 胡柯青 on 2022-01-05.
//

#ifndef JUDGE_HEARTBEAT_REQUEST_H
#define JUDGE_HEARTBEAT_REQUEST_H

#include "../request.h"

class HeartbeatRequest : public Request {
public:
    HeartbeatRequest(int threadAccumulation, int socketAccumulation);

    void updateThread(int newValue);

    void updateSocket(int newValue);
};

/// region define

HeartbeatRequest::HeartbeatRequest(int threadAccumulation, int socketAccumulation) : Request(Api::Heartbeat) {
    this->set(constant.threadAccumulation, to_string(threadAccumulation));
    this->set(constant.socketAccumulation, to_string(socketAccumulation));
}

void HeartbeatRequest::updateThread(int newValue) {
    this->set(constant.threadAccumulation, to_string(newValue));
}

void HeartbeatRequest::updateSocket(int newValue) {
    this->set(constant.socketAccumulation, to_string(newValue));
}

/// endregion

#endif //JUDGE_HEARTBEAT_REQUEST_H
