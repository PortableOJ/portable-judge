//
// Created by 胡柯青 on 2021-10-10.
//

#ifndef JUDGE_LOGGER_H
#define JUDGE_LOGGER_H

#include "include.h"
#include "mutex.h"

class Logger {
private:
    bool fileMode;
    ostream *out;
    char timeStr[20];
    static Logger *logger;
    Mutex<bool> lock;

    Logger();

    explicit Logger(const string &file);

    void print(const string &msg, int offset);

    template<class T>
    void print(const string &msg, int offset, const T &t);

    template<class T, class ...Args>
    void print(const string &msg, int offset, const T &t, const Args &...args);

    template<class ...Args>
    void print(const string &level, const string &msg, const Args &...args);

public:
    static void init();

    static void close();

    static char *timer();

    template<class ...Args>
    static void err(const string &msg, const Args &...args);

    template<class ...Args>
    static void warn(const string &msg, const Args &...args);

    template<class ...Args>
    static void info(const string &msg, const Args &...args);

    template<class ...Args>
    static void debug(const string &msg, const Args &...args);

    template<class ...Args>
    static void trace(const string &msg, const Args &...args);
};

/// region define

Logger *Logger::logger = nullptr;

Logger::Logger() :fileMode(false), out(&cout), timeStr() {}

Logger::Logger(const string &file) : fileMode(true), out(new ofstream(file)), timeStr() {}

void Logger::print(const string &msg, int offset) {
    for (int i = offset; i < msg.size(); ++i) *Logger::out << msg[i];
    *Logger::out << endl;
#if DEBUG_LEVEL <= 1
    Logger::out->flush();
#endif
}

template<class T>
void Logger::print(const string &msg, int offset, const T &t) {
    for (int i = offset; i < msg.size(); ++i) {
        if (msg[i] == '%') {
            *Logger::out << t;
            Logger::print(msg, i + 1);
            return;
        } else {
            *Logger::out << msg[i];
        }
    }
}

template<class T, class... Args>
void Logger::print(const string &msg, int offset, const T &t, const Args &... args) {
    for (int i = offset; i < msg.size(); ++i) {
        if (msg[i] == '%') {
            *Logger::out << t;
            Logger::print(msg, i + 1, args...);
            return;
        } else {
            *Logger::out << msg[i];
        }
    }
}

template<class... Args>
void Logger::print(const string &level, const string &msg, const Args &... args) {
    lock.run([&](bool &) {
        char *format = new char[100];
        sprintf(format,
                "%s %6s [%20lu] : ",
                timer(),
                level.data(),
                (unsigned long) pthread_self()
        );
        *Logger::out << format;
        delete[] format;
        print(msg, 0, args...);
    });
}

void Logger::init() {
    if (logger == nullptr) {
        const char *logPath = getenv("log");
        if (logPath != nullptr) {
            logger = new Logger(logPath);
        } else {
            logger = new Logger();
        }
    }
}

void Logger::close() {
    if (logger == nullptr) return;
    logger->out->flush();
    if (logger->fileMode) {
        auto *outPtr = dynamic_cast<ifstream *>(logger->out);
        outPtr->close();
        delete logger->out;
    }
    delete logger;
    logger = nullptr;
}

char *Logger::timer() {
    time_t times = time(nullptr);
    struct tm *utcTime = gmtime(&times);
    sprintf(logger->timeStr,
            "%04d-%02d-%02d %02d:%02d:%02d",
            utcTime->tm_year + 1900,
            utcTime->tm_mon + 1,
            utcTime->tm_mday,
            utcTime->tm_hour,
            utcTime->tm_min,
            utcTime->tm_sec);
    return logger->timeStr;
}

template<class... Args>
void Logger::err(const string &msg, const Args &... args) {
#if DEBUG_LEVEL <= 4
    Logger::logger->print("ERROR", msg, args...);
#endif
}

template<class... Args>
void Logger::warn(const string &msg, const Args &... args) {
#if DEBUG_LEVEL <= 3
    Logger::logger->print("WARN", msg, args...);
#endif
}

template<class... Args>
void Logger::info(const string &msg, const Args &... args) {
#if DEBUG_LEVEL <= 2
    Logger::logger->print("INFO", msg, args...);
#endif
}

template<class... Args>
void Logger::debug(const string &msg, const Args &... args) {
#if DEBUG_LEVEL <= 1
    Logger::logger->print("DEBUG", msg, args...);
#endif
}

template<class... Args>
void Logger::trace(const string &msg, const Args &... args) {
#if DEBUG_LEVEL <= 0
    Logger::logger->print("TRACE", msg, args...);
#endif
}

/// endregion

#endif //JUDGE_LOGGER_H
