//
// Created by keqing on 2021-10-10.
//

#ifndef JUDGE_INCLUDE_H
#define JUDGE_INCLUDE_H

#include <pthread.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>

#include <map>
#include <set>
#include <string>
#include <vector>
#include <utility>
#include <filesystem>
#include <unordered_map>

#include <wait.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <seccomp.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <bits/stdc++.h>
#include <sys/resource.h>

using namespace std;
using namespace std::filesystem;

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

#endif //JUDGE_INCLUDE_H
