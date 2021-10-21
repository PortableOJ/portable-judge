#include "thread/threadPool.h"
#include "socket/session.h"
#include "env/env.h"

int main() {
    /// region logger

    Logger::init();

    /// region env

    Env::init(constant.envFileName);
    Env *env = Env::ctx();

    /// region thread pool

    auto *pool = new ThreadPool(env->getInt(constant.maxThreadCore));
    env->set(constant.workThreadPool, pool);

//    Session session("localhost", 10000);
    Session session("10.21.234.123", 10000);
    Request request(Api::Register);
    Callback callback(nullptr, [&](void *, stringstream &ss) {
        string s;
        ss >> s;
        Logger::info("%s", s.c_str());
    });

    session.send(request, callback);
    session.close();

    pool->wait();
    delete pool;

    /// endregion

    Env::close();

    /// endregion

    Logger::close();

    /// endregion
}

int tmain() {
    sockaddr_in sockAddr{};

    if (inet_pton(AF_INET, "10.21.234.123", &sockAddr.sin_addr) > 0) {
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_port = htons(10000);
    }
    int socketId = socket(sockAddr.sin_family, SOCK_STREAM, 0);
    if (socketId < 0) throw runtime_error("创建网络套接字失败");
    if (connect(socketId, (sockaddr *) &sockAddr, sizeof(sockAddr)) < 0) throw runtime_error("网络连接失败");

    thread t([&]() {
        while (true) {
            char buffer[1000];
            long long n = read(socketId, buffer, 900);
            if (n <= 0) break;
            buffer[n] = 0;
            cerr << "n = " << n << endl;
            cout << buffer << endl;
        }
    });

    while (true) {
        string data;
        cin >> data;
        if (data == "END") break;
        write(socketId, data.c_str(), data.size());
        write(socketId, "\n", 1);
//        shutdown(socketId, SHUT_WR);
    }
    pthread_kill(t.native_handle(), SIGKILL);
    t.join();
}
