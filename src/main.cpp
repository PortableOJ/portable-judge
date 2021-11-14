#include "thread/__init__.h"
#include "socket/__init__.h"
#include "compiler/__init__.h"
#include "file/__init__.h"
#include "runner/__init__.h"

int main() {
    Logger::init();

    int input[2], output[2], error[2];

    pipe(input);
    pipe(output);
    pipe(error);

    Report report{};

    CppRunner cppRunner;
    JudgeResultEnum resultEnum = cppRunner.run("./src/test/tmp/main.cpp", input, output, error,
                                               1, 100000000ul, &report, false);

    Logger::info("result: %", (int) resultEnum);

    close(output[1]);
    char buffer[100] = {0};
    int n = read(output[0], buffer, 100);
    buffer[n] = 0;
    Logger::info("output: %", buffer);

    Logger::close();
}
