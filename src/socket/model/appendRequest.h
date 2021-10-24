//
// Created by keqing on 2021-10-24.
//

#ifndef JUDGE_APPEND_REQUEST_H
#define JUDGE_APPEND_REQUEST_H

#include "../request.h"

class AppendRequest : public Request {
public:
    explicit AppendRequest();
};

/// region define

AppendRequest::AppendRequest() : Request(Api::Append) {
    this->set(constant.judgeCode, *Env::ctx()->getString(constant.judgeCode));
}

/// endregion

#endif //JUDGE_APPEND_REQUEST_H
