//
// Created by 胡柯青 on 2021-10-29.
//

#include "../util/__init__.h"

int main() {
    Logger::init();

    Logger::info("% = %", 123, "abc");

    Logger::close();
}
