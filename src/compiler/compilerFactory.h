//
// Created by 胡柯青 on 2021-11-05.
//

#ifndef JUDGE_COMPILER_FACTORY_H
#define JUDGE_COMPILER_FACTORY_H

#include "derive/cCompiler.h"
#include "derive/cppCompiler.h"
#include "../language/__init__.h"

class CompilerFactory {
    const static Compiler *c;
    const static Compiler *cpp;
public:
    const static Compiler *getCompiler(const Language &lang);
};

/// region define

const Compiler *CompilerFactory::c = new CCompiler();       // NOLINT
const Compiler *CompilerFactory::cpp = new CppCompiler();   // NOLINT

const Compiler *CompilerFactory::getCompiler(const Language &lang) {
    if (lang.getLang() == "C") return c;
    if (lang.getLang() == "CPP") return cpp;
    return nullptr;
}

/// endregion

#endif //JUDGE_COMPILER_FACTORY_H
