cmake_minimum_required(VERSION 3.16)
project(judge)

set(CMAKE_CXX_STANDARD 17)

add_executable(main src/main.cpp

        src/env/env.h

        src/socket/callback.h
        src/socket/request.h
        src/socket/session.h

        src/thread/job.h
        src/thread/threadPool.h

        src/util/__init__.h
        src/util/include.h
        src/util/logger.h
        src/util/mutex.h
        )

target_link_libraries(main seccomp)

find_package(Threads)
target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})
