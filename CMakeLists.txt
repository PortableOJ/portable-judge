cmake_minimum_required(VERSION 3.16)
project(judge)

set(CMAKE_CXX_STANDARD 17)

add_executable(main
        src/main.cpp

        src/compiler/__init__.h
        src/compiler/compiler.h
        src/compiler/compilerFactory.h

        src/compiler/derive/cCompiler.h
        src/compiler/derive/cppCompiler.h

        src/env/__init__.h
        src/env/env.h

        src/file/__init__.h
        src/file/fileManager.h

        src/language/__init__.h
        src/language/language.h

        src/runner/__init__.h
        src/runner/result.h
        src/runner/runner.h
        src/runner/runnerFactory.h

        src/runner/derive/cRunner.h
        src/runner/derive/cppRunner.h

        src/socket/__init__.h
        src/socket/callback.h
        src/socket/request.h
        src/socket/session.h
        src/socket/sessionPool.h
        src/socket/socketWork.h

        src/socket/model/appendRequest.h
        src/socket/model/heartbeatRequest.h
        src/socket/model/problemDataInRequest.h
        src/socket/model/problemDataOutRequest.h
        src/socket/model/problemJudgeCodeRequest.h
        src/socket/model/registerRequest.h
        src/socket/model/solutionCodeRequest.h
        src/socket/model/solutionInfoRequest.h
        src/socket/model/solutionReportRequest.h
        src/socket/model/standardJudgeCodeRequest.h
        src/socket/model/standardJudgeRequest.h

        src/thread/__init__.h
        src/thread/job.h
        src/thread/task.h
        src/thread/threadPool.h

        src/util/__init__.h
        src/util/countMutex.h
        src/util/include.h
        src/util/logger.h
        src/util/mutex.h
        src/util/timeoutMutex.h
        src/util/util.h

        src/work/judgeWork.h
        src/work/testWork.h
        src/work/__init__.h)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")

    target_link_libraries(main seccomp)

endif ()

find_package(Threads)
target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})
