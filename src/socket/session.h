//
// Created by keqing on 2021-10-14.
//

#ifndef JUDGE_SESSION_H
#define JUDGE_SESSION_H

#include <utility>

#include "request.h"
#include "callback.h"

class Session {
    string host;
    int port;
    int socketId;
    sockaddr_in sockAddr;

    void init();

public:
    Session(string host, int port);

    void send(Request *request, Callback *callback) const;

    void close() const;
};

/// region define

void Session::init() {
    if (inet_pton(AF_INET, host.c_str(), &sockAddr.sin_addr) > 0) {
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_port = htons(port);
    } else {
        hostent *net = gethostbyname(host.c_str());
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_addr = *(struct in_addr *) net->h_addr_list[0];
        sockAddr.sin_port = htons(port);
    }
    socketId = socket(sockAddr.sin_family, SOCK_STREAM, 0);
    if (socketId < 0) {
        Logger::err("Socket create fail");
        exit(-1);
    }
    if (connect(socketId, (sockaddr *) &sockAddr, sizeof(sockAddr)) < 0) {
        Logger::err("Network Fail");
        exit(-1);
    }
}

Session::Session(string host, int port)
        : host(move(host)), port(port), socketId(-1), sockAddr() {
    init();
}

void Session::send(Request *request, Callback *callback) const {
    request->send(socketId);
    callback->exec(socketId);
}

void Session::close() const {
    shutdown(socketId, SHUT_WR);
}

/// endregion

#endif //JUDGE_SESSION_H
