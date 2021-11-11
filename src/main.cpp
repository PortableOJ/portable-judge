#include "thread/__init__.h"
#include "socket/__init__.h"
#include "compiler/__init__.h"
#include "file/__init__.h"

int main() {
    Logger::init();

    Env::init(constant.envFileName);
    Env *env = Env::ctx();

    string code = "MyCode";
    env->set(constant.judgeCode, &code);

    auto *socketPool = new SessionPool(env->getInt(constant.initSocketCore));
    auto *workPool = new ThreadPool(env->getInt(constant.initThreadCore));
    socketPool->init();
    workPool->init();

    FileManager fileManager;
    cout << fileManager.init(socketPool, workPool) << endl;

    socketPool->wait();
    delete socketPool;

    workPool->wait();
    delete workPool;

    env->close();

    Logger::close();
}
