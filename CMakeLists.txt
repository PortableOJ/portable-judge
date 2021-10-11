cmake_minimum_required(VERSION 3.16)
project(judge)

set(CMAKE_CXX_STANDARD 17)

add_executable(main src/main.cpp

        src/util/include.h

        src/util/thread.h
        src/util/process.h
        src/util/logger.h)
target_link_libraries(main seccomp)

find_package(Threads)
target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})