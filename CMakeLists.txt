cmake_minimum_required(VERSION 3.16)
project(judge)

set(CMAKE_CXX_STANDARD 17)

add_executable(main src/main.cpp

        src/util/include.h

        src/thread/threadPool.h
        src/thread/job.h

        src/util/__init__.h
        src/util/logger.h
        src/util/env.h
        src/util/mutex.h
        )

target_link_libraries(main seccomp)

find_package(Threads)
target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})