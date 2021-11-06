//
// Created by keqing on 2021-10-12.
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
        return true;
    };

    auto toBool = [&](Node &node) {
        if (value == "true") {
            node.b = true;
            return true;
        } else if (value == "false") {
            node.b = false;
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
        return true;
    };

    auto toPtr = [&](Node &node) {
        auto *ptr = new string(value);
        node.ptr = ptr;
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
    const string serverUrl = "serverUrl";
    const string serverPort = "serverPort";
    const string serverCode = "serverCode";


    /// endregion

    /// region static

    const string workThreadPool = "workThreadPool";
    const string workSocketPool = "workSocketPool";
    const string judgeCode = "judgeCode";

    /// endregion

    /// region language

    const string c = "C";
    const string cExtension = ".c";
    const string cpp = "CPP";
    const string cppExtension = ".cpp";

    /// endregion

} constant; // NOLINT

#endif //JUDGE_ENV_H
