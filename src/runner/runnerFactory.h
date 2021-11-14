//
// Created by 胡柯青 on 2021-11-14.
//

#ifndef JUDGE_RUNNER_FACTORY_H
#define JUDGE_RUNNER_FACTORY_H

#include "derive/cppRunner.h"
#include "derive/cRunner.h"

class RunnerFactory {
public:
    const static Runner *getRunner(const Language &language);
};

/// region define

const Runner *RunnerFactory::getRunner(const Language &lang) {
    if (lang.getLang() == "C") return new CRunner();
    if (lang.getLang() == "CPP") return new CppRunner();
    return nullptr;
}

/// endregion

#endif //JUDGE_RUNNER_FACTORY_H
