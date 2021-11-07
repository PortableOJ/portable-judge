#include "thread/__init__.h"
#include "socket/__init__.h"
#include "compiler/__init__.h"

int main() {
    Logger::init();

    const Compiler *compiler = CompilerFactory::getCompiler(constant.cpp);
    bool res = compiler->compile("./src/test/tmp", "main", "-std=c++11");
    Logger::info("%", res);

    Logger::close();
}
