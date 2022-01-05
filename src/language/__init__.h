//
// Created by 胡柯青 on 2021-11-11.
//

#ifndef JUDGE_LANGUAGE_INIT_H
#define JUDGE_LANGUAGE_INIT_H

#include "language.h"

Language C89("C89", "C", "-std=gnu89", "main", ".c");           // NOLINT
Language C99("C89", "C", "-std=gnu99", "main", ".c");           // NOLINT
Language C11("C11", "C", "-std=gnu11", "main", ".c");           // NOLINT

Language CPP98("CPP98", "CPP", "-std=c++98", "main", ".cpp");   // NOLINT
Language CPP11("CPP11", "CPP", "-std=c++11", "main", ".cpp");   // NOLINT
Language CPP14("CPP14", "CPP", "-std=c++14", "main", ".cpp");   // NOLINT

Language &Judge = CPP11;

#endif //JUDGE_LANGUAGE_INIT_H
