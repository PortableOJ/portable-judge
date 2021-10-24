cmake_minimum_required(VERSION 3.16)
project(judge)

set(CMAKE_CXX_STANDARD 17)

add_executable(main
        src/main.cpp

        src/env/__init__.h
        src/env/env.h

        src/socket/__init__.h
        src/socket/callback.h
        src/socket/request.h
        src/socket/session.h
        src/socket/sessionPool.h

        src/thread/__init__.h
        src/thread/job.h
        src/thread/threadPool.h

        src/util/__init__.h
        src/util/include.h
        src/util/logger.h
        src/util/mutex.h
        src/thread/task.h src/socket/socketWork.h src/socket/model/registerRequest.h src/socket/model/appendRequest.h src/util/countMutex.h src/socket/model/closeRequest.h)

target_link_libraries(main seccomp)

find_package(Threads)
target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})
