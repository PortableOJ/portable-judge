//
// Created by keqing on 2021-10-10.
//

#ifndef JUDGE_INCLUDE_H
#define JUDGE_INCLUDE_H

#include <pthread.h>

#include <ctime>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>

#include <map>
#include <set>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>
#include <utility>
#include <istream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <functional>
#include <unordered_map>
#include <condition_variable>

#ifdef __linux__
#include <wait.h>
#endif

#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#ifdef __linux__
#include <seccomp.h>
#endif

#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/resource.h>

using namespace std;
using namespace std::filesystem;

typedef unsigned long id;

namespace STD {
    const unsigned long B = 1;
    const unsigned long KB = 1024 * B;
    const unsigned long MB = 1024 * KB;
    const unsigned long GB = 1024 * MB;

    const int ms = 1;
    const int s = 1000 * ms;
    const int m = 60 * s;
    const int h = 60 * m;
}

#ifdef __APPLE__

// 在 mac 下模拟伪造拥有系统调用环境保护，仅适用于测试整体流程，对安全性测试应当在 Linux 下进行

// class
#define scmp_filter_ctx int
#define scmp_datum_t void*

// function
#define SCMP_SYS(a) 0
#define SCMP_A0(a, b) 0
#define seccomp_rule_add(a, b, c, d, ...) 0
#define seccomp_init(a) 0
#define seccomp_load(a) 0
#define seccomp_release(a) 0

// value
#define SCMP_ACT_KILL 0
#define SCMP_ACT_ALLOW 0
#define SCMP_CMP_EQ 0

#endif

#endif //JUDGE_INCLUDE_H
