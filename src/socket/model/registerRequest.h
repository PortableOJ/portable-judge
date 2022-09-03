//
// Created by 胡柯青 on 2021-10-24.
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
    Env *env = Env::ctx();
    this->set(constant.serverCode, *env->getString(constant.serverCode));
    this->set(constant.maxSocketCore, env->getInt(constant.initSocketCore));
    this->set(constant.maxWorkCore, env->getInt(constant.initWorkCore));
}

/// endregion

#endif //JUDGE_REGISTER_REQUEST_H
