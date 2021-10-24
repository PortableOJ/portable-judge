//
// Created by keqing on 2021-10-24.
//

#ifndef JUDGE_CLOSE_REQUEST_H
#define JUDGE_CLOSE_REQUEST_H

#include "../request.h"

class CloseRequest : public Request {
public:
    explicit CloseRequest();
};

/// region define

CloseRequest::CloseRequest() : Request(Api::Close) {}

/// endregion

#endif //JUDGE_CLOSE_REQUEST_H
