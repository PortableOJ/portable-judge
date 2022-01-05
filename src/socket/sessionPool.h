//
// Created by 胡柯青 on 2021-10-22.
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
    return [&]() {
        Env *env = Env::ctx();
        Session session(*env->getString(constant.serverUrl), env->getInt(constant.serverPort));

        /// region append

        {
            AppendRequest appendRequest;
            Callback callback(nullptr, [&](void *, stringstream &ss) {});
            if (!session.send(&appendRequest, &callback)) {
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
                if (!session.send(socketWork->getRequest(), socketWork->getCallback())) {
                    Logger::err("socket response fail, method: %", socketWork->getRequest()->getMethod());
                } else {
                    cur->exec();
                    delete cur;
                }
            } else {
                if (sleep()) {
                    break;
                }
            }
        }

        /// endregion

        /// region close

        {
//            CloseRequest closeRequest;
//            Callback callback(nullptr, [&](void *, stringstream &ss) {});
//            session.send(&closeRequest, &callback);
            session.close();
        }

        /// endregion

        killSelf();
    };
}

SessionPool::SessionPool(int core) : ThreadPool(core) {}

/// endregion

#endif //JUDGE_SESSION_POOL_H
