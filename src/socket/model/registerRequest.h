//
// Created by keqing on 2021-10-24.
//

#ifndef JUDGE_REGISTER_REQUEST_H
#define JUDGE_REGISTER_REQUEST_H

#include "../request.h"

class RegisterRequest : public Request {
public:
    RegisterRequest();
};

/// region define

RegisterRequest::RegisterRequest() : Request(Api::Register) {
    this->set(constant.serverCode, *Env::ctx()->getString(constant.serverCode));
}

/// endregion

#endif //JUDGE_REGISTER_REQUEST_H
