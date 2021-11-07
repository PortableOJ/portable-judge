//
// Created by keqing on 2021-11-06.
//

#include "../compiler/__init__.h"

int main() {
    Logger::init();

    Compiler *compiler = CompilerFactory::getCompiler(constant.cpp);
    compiler->compile("./src/test/")

    Logger::close();
}