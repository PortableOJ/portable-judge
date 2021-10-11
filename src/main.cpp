#include "util/logger.h"
#include "util/process.h"
#include "util/thread.h"

int main() {
    Logger::init();
    Thread::init();

    Logger::info("test % test", 123);

    Thread::close();
    Logger::close();
}
