//
// Created by HKQ on 2021-12-23.
//

#include "../runner/__init__.h"

int main () {
    const Runner *runner = RunnerFactory::getRunner(C11);
    runner->run("/Users/shiroha/portable/test/3/main", nullptr, nullptr, nullptr, 1000, 512000000L, "", nullptr, false);
}