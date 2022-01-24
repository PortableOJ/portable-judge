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

const char *judgeResultEnumString[] = {
        "ACCEPT",
        "SYSTEM_ERROR",
        "RUNTIME_ERROR",
        "TIME_LIMIT_EXCEEDED",
        "SEGMENT_FAIL",
        "ERRONEOUS_ARITHMETIC_OPERATION",
        "ILLEGAL_SYSTEM_CAL",
        "RETURN_NOT_ZERO",
        "COMPILE_ERROR",
        "JUDGE_COMPILE_ERROR",
        "JUDGE_FAIL",
        "WRONG_ANSWER"
};

#endif //JUDGE_RESULT_H
