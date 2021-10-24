//
// Created by keqing on 2021-10-22.
//

#ifndef JUDGE_SESSION_POOL_H
#define JUDGE_SESSION_POOL_H

#include "../thread/__init__.h"
#include "model/appendRequest.h"
#include "model/closeRequest.h"
#include "../env/__init__.h"
#include "session.h"
#include "socketWork.h"

class SessionPool : public ThreadPool {
protected:
    function<void()> workFunc() override;

public:
    explicit SessionPool(int core);
};

/// region define

function<void()> SessionPool::workFunc() {
    Logger::trace("Session");
    return [&]() {
        Env *env = Env::ctx();
        Session session(*env->getString(constant.serverUrl), env->getInt(constant.serverPort));

        /// region append

        {
            bool connectFail = false;
            AppendRequest appendRequest;
            Callback callback(nullptr, [&](void *, stringstream &ss) {
                string str;
                ss >> str;
                if (str != "OK") {
                    connectFail = true;
                }
            });
            session.send(&appendRequest, &callback);
            if (connectFail) {
                killSelf();
                return;
            }
        }

        /// endregion

        /// region work

        while (true) {
            Job *cur = takeJob();
            if (cur != nullptr) {
                auto *socketWork = dynamic_cast<SocketWork *>(cur);
                session.send(socketWork->getRequest(), socketWork->getCallback());
                cur->exec();
                delete cur;
            } else {
                if (sleep()) {
                    break;
                }
            }
        }

        /// endregion

        /// region close

        {
            CloseRequest closeRequest;
            Callback callback(nullptr, [&](void *, stringstream &ss) {});
            session.send(&closeRequest, &callback);
            session.close();
        }

        /// endregion

        killSelf();
    };
}

SessionPool::SessionPool(int core) : ThreadPool(core) {}

/// endregion

#endif //JUDGE_SESSION_POOL_H
