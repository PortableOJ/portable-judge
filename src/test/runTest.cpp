//
// Created by HKQ on 2021-12-23.
//

#include "../runner/__init__.h"

int main() {
    chdir("/Users/shiroha/portable");
    const Runner *runner = RunnerFactory::getRunner(C11);
    int in[2]{-1, -1};
    in[0] = open("problem/3/example.in", O_RDONLY);
    runner->runCode("test/3/main", in, nullptr, nullptr, 1000, 134217728L, "", 1, false);
}