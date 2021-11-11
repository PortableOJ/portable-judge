//
// Created by keqing on 2021-11-06.
//

#include "../compiler/__init__.h"

int main() {
    Logger::init();

    const Compiler *compiler = CompilerFactory::getCompiler(CPP11);
    bool res = compiler->compile("./src/test/tmp/main.cpp", CPP11.getParams());
    Logger::info("%", res);

    Logger::close();

}
