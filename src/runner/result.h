//
// Created by 胡柯青 on 2021-11-11.
//

#ifndef JUDGE_RESULT_H
#define JUDGE_RESULT_H

#include "../util/__init__.h"

enum class JudgeResultEnum : int {
    Accept = 0,
    SystemError = 1,
    RuntimeError = 2,
    TimeLimitExceeded = 3,
    SegmentFail = 4,
    ErroneousArithmeticOperation = 5,
    IllegalSystemCall = 6,
    ReturnNotZero = 7,
    CompileError = 8,
    JudgeCompileError = 9,
    JudgeFail = 10,
    WrongAnswer = 11,
};

#endif //JUDGE_RESULT_H
