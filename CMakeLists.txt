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

        src/socket/__init__.h
        src/socket/callback.h
        src/socket/request.h
        src/socket/session.h
        src/socket/sessionPool.h
        src/socket/socketWork.h

        src/socket/model/appendRequest.h
        src/socket/model/closeRequest.h
        src/socket/model/registerRequest.h

        src/thread/__init__.h
        src/thread/job.h
        src/thread/task.h
        src/thread/threadPool.h

        src/util/__init__.h
        src/util/countMutex.h
        src/util/include.h
        src/util/logger.h
        src/util/mutex.h
        src/util/timeoutMutex.h)

target_link_libraries(main seccomp)

find_package(Threads)
target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})
