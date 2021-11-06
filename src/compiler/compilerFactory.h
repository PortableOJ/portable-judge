//
// Created by keqing on 2021-11-05.
//

#ifndef JUDGE_COMPILER_FACTORY_H
#define JUDGE_COMPILER_FACTORY_H

#include "derive/cCompiler.h"
#include "derive/cppCompiler.h"

class compilerFactory {
    const static Compiler *c;
    const static Compiler *cpp;

    const static Compiler *getCompiler(const string &str);
};

/// region define


const Compiler *compilerFactory::c = new CCompiler();       // NOLINT
const Compiler *compilerFactory::cpp = new CppCompiler();   // NOLINT

const Compiler *compilerFactory::getCompiler(const string &str) {
    if (str == constant.c) return c;
    if (str == constant.cpp) return cpp;
    return nullptr;
}


/// endregion

#endif //JUDGE_COMPILER_FACTORY_H
