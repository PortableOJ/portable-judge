//
// Created by 胡柯青 on 2021-10-12.
//

#ifndef JUDGE_ENV_H
#define JUDGE_ENV_H

#include "../util/include.h"

class Env {
    union Node {
        char c;
        bool b;
        int i;
        void *ptr;
/// region define

        explicit Node(char c) : c(c) {}

        explicit Node(bool b) : b(b) {}

        explicit Node(int i) : i(i) {}

        explicit Node(void *ptr) : ptr(ptr) {}

/// endregion
    };

    unordered_map<string, Node> data;

    static vector<string *> *needFree;
    static Env *localThread;

    static Node getType(const string &value);

public:
    static Env *ctx();

    static void init(const string &file);

    explicit Env(const string &file);

    template<typename T>
    void set(const string &key, const T &value);

    char getChar(const string &key);

    bool getBool(const string &key);

    int getInt(const string &key);

    const string *getString(const string &key);

    template<typename T>
    const T *getPtr(const string &key);

    void remove(const string &key);

    static void close();
};

/// region define

Env *Env::localThread = nullptr;
vector<string *> *Env::needFree = nullptr;

Env::Node Env::getType(const string &value) {
    auto toChar = [&](Node &node) {
        if (value.size() != 1) return false;
        node.c = value[0];
        Logger::debug("get env value: %", node.c);
        return true;
    };

    auto toBool = [&](Node &node) {
        if (value == "true") {
            node.b = true;
            Logger::debug("get env value: true");
            return true;
        } else if (value == "false") {
            node.b = false;
            Logger::debug("get env value: false");
            return true;
        }
        return false;
    };

    auto toInt = [&](Node &node) {
        int tmp = 0;
        bool flag = false;
        if (value[0] == '-') flag = true;
        for (int i = flag ? 1 : 0; i < value.size(); ++i) {
            if (value[i] < '0' || value[i] > '9') return false;
            tmp *= 10;
            tmp += value[i] - '0';
        }
        node.i = tmp * (flag ? -1 : 1);
        Logger::debug("get env value: %", node.i);
        return true;
    };

    auto toPtr = [&](Node &node) {
        auto *ptr = new string(value);
        node.ptr = ptr;
        Logger::debug("get env value: %", value);
        return true;
    };

    Node node(0);
    if (toInt(node) || toBool(node) || toChar(node)) {
        return node;
    }
    toPtr(node);
    needFree->push_back((string *) node.ptr);
    return node;
}

Env *Env::ctx() {
    return localThread;
}

void Env::init(const string &file) {
    needFree = new vector<string *>();
    localThread = new Env(file);
}

Env::Env(const string &file) {
    ifstream in(file);
    string key, value;
    while (getline(in, key, '=') && getline(in, value)) {
        if (key.empty()) continue;
        Logger::debug("get env key: %", key);
        const char *envValue = getenv(key.c_str());
        if (envValue != nullptr) {
            value = envValue;
        }
        Node type = getType(value);
        data.insert({key, type});
    }
    in.close();
}

template<typename T>
void Env::set(const string &key, const T &value) {
    data.insert({key, Node(value)});
}

char Env::getChar(const string &key) {
    auto dataIter = data.find(key);
    if (dataIter != data.end()) return dataIter->second.c;
    return (char) 0;
}

bool Env::getBool(const string &key) {
    auto dataIter = data.find(key);
    if (dataIter != data.end()) return dataIter->second.b;
    return false;
}

int Env::getInt(const string &key) {
    auto dataIter = data.find(key);
    if (dataIter != data.end()) return dataIter->second.i;
    return 0;
}

const string *Env::getString(const string &key) {
    auto dataIter = data.find(key);
    if (dataIter != data.end()) return (string *) (dataIter->second.ptr);
    return nullptr;
}

template<typename T>
const T *Env::getPtr(const string &key) {
    auto dataIter = data.find(key);
    if (dataIter != data.end()) return (T *) (dataIter->second.ptr);
    return nullptr;
}

void Env::remove(const string &key) {
    data.erase(key);
}

void Env::close() {
    for (auto &item: *needFree) delete item;
    delete localThread;
    delete needFree;
}

/// endregion

struct Constant {
    const string envFileName = "env";

    /// region env

    const string initThreadCore = "initThreadCore";
    const string initSocketCore = "initSocketCore";
    const string initWorkCore = "initWorkCore";

    const string localStorage = "localStorage";
    const string home = "home";


    const string serverUrl = "serverUrl";
    const string serverPort = "serverPort";
    const string serverCode = "serverCode";

    const string compileMaxTime = "compileMaxTime";
    const string compileRealMaxTime = "compileRealMaxTime";
    const string compileMaxMemory = "compileMaxMemory";
    const string compileFileSize = "compileFileSize";
    const string runningRealMaxTime = "runningRealMaxTime";

    const string heartbeatTime = "heartbeatTime";

    /// endregion

    /// region static

    const string judgeCode = "judgeCode";           // 本 judge 的编号

    const string terminate = "terminate";           // 终止命令

    const string maxThreadCore = "maxThreadCore";           // 最大线程池核心线程数
    const string maxSocketCore = "maxSocketCore";           // 最大网络池核心线程数
    const string maxWorkCore = "maxWorkCore";               // 最大任务池核心线程数

    const string threadAccumulation = "threadAccumulation"; // 线程池堆积
    const string socketAccumulation = "socketAccumulation"; // 连接池堆积
    const string workAccumulation = "workAccumulation";     // 连接池堆积

    const string name = "name";                     // 名称
    const string id = "id";                         // ID
    const string msg = "msg";                       // msg
    const string value = "value";                   // value
    const string judge = "judge";                   // judge
    const string flag = "flag";                     // judge
    const string pos = "pos";                       // type
    const string testName = "testName";             // testName
    const string timeCost = "timeCost";             // timeCost
    const string memoryCost = "memoryCost";         // memoryCost

    const string useDiyJudge = "DIY";               // 使用非默认程序
    const string testlib = "testlib.h";             // testlib 的保存文件名

    /// endregion

    /// region heartbeat

    const string judgeTask = "judgeTask";           // 一个 judge 任务
    const string testTask = "testTask";             // 一个 test 任务
    const string threadCore = "threadCore";         // 更新线程池的核心数
    const string socketCore = "socketCore";         // 更新网络连接池的核心数
    const string workCore = "workCore";             // 更新任务池的核心数
    const string cleanProblem = "cleanProblem";     // 清理问题缓存
    const string cleanJudge = "cleanJudge";         // 清理问题缓存

    /// endregion

    /// region Solution Info

    const string problemId = "problemId";
    const string language = "language";
    const string judgeName = "judgeName";
    const string testNum = "testNum";
    const string timeLimit = "timeLimit";
    const string memoryLimit = "memoryLimit";

    /// endregion

} constant; // NOLINT

#endif //JUDGE_ENV_H
