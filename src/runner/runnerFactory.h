//
// Created by 胡柯青 on 2021-11-14.
//

#ifndef JUDGE_RUNNER_FACTORY_H
#define JUDGE_RUNNER_FACTORY_H

#include "derive/cppRunner.h"
#include "derive/cRunner.h"

class RunnerFactory {
private:

    const static Runner *c;
    const static Runner *cpp;


public:

    static const Runner *getRunner(const Language &language);
};

/// region define

const Runner *RunnerFactory::c = new CRunner();       // NOLINT
const Runner *RunnerFactory::cpp = new CppRunner();   // NOLINT

const Runner *RunnerFactory::getRunner(const Language &lang) {
    if (lang.getLang() == "C") return c;
    if (lang.getLang() == "CPP") return cpp;
    return nullptr;
}

/// endregion

#endif //JUDGE_RUNNER_FACTORY_H
