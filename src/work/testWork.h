//
// Created by 胡柯青 on 2021-11-14.
//

#ifndef JUDGE_TEST_WORK_H
#define JUDGE_TEST_WORK_H

#include "../thread/__init__.h"
#include "../file/__init__.h"
#include "../compiler/__init__.h"
#include "../runner/__init__.h"
#include "../socket/__init__.h"

class TestWork {
private:

    bool fail;
    path codePath;
    string testName;
    JudgeResultEnum codeRunningResult;

    CountMutex cm;

    const Language *language;
    const Compiler *compiler;
    const Runner *runner;

    id problemId;
    int testNum;
    unsigned long timeLimit;
    unsigned long memoryLimit;

    path testInPath;
    path testOutPath;

    int pipes[3][2]{};

    SessionPool *sessionPool;
    ThreadPool *threadPool;

    bool init();

    void run();

    void clean() const;

public:
    TestWork(id problemId, ThreadPool *tp, SessionPool *sp);

    void start();

};

/// region define

bool TestWork::init() {
    cm.reset(1);
    TestInfoRequest testInfoRequest(problemId);
    Callback callback(this, [](void *data, stringstream &ss) {
        auto testWork = (TestWork *) data;
        string key;
        while (ss >> key) {
            if (key == constant.language) {
                string tmp;
                ss >> tmp;
                testWork->language = Language::getLanguage(tmp);
            } else if (key == constant.testNum) {
                ss >> testWork->testNum;
            } else if (key == constant.timeLimit) {
                ss >> testWork->timeLimit;
            } else if (key == constant.memoryLimit) {
                ss >> testWork->memoryLimit;
            }
        }
    }, [](void *data) {
        ((TestWork *) data)->fail = true;
    });
    auto socketWork = new SocketWork(&testInfoRequest, &callback, &cm);
    sessionPool->submit(socketWork);
    cm.wait();
    if (fail) {
        return false;
    }
    FileManager::createTestDir(problemId);
    return true;
}

void TestWork::run() {

    /// region 编译

    codePath = FileManager::createTestCode(problemId, *language);
    compiler = CompilerFactory::getCompiler(*language);
    runner = RunnerFactory::getRunner(*language);
    fail = !compiler->compile(codePath, language->getParams());
    if (fail) {
        TestCompileReportRequest testCompileReportRequest(problemId, !fail);
        Callback compileMsgCallback(this, [](void *data, stringstream &ss) {}, [](void *data) {
            ((TestWork *) data)->fail = true;
        });
        cm.reset(1);
        auto socketWork = new SocketWork(&testCompileReportRequest, &compileMsgCallback, &cm);
        sessionPool->submit(socketWork);
        cm.wait();
        return;
    }

    /// endregion

    TestTestRequest testRequest(problemId);
    Callback testNameCallback(this, [](void *data, stringstream &ss) {
        ss >> ((TestWork *) data)->testName;
    }, [](void *data) {
        ((TestWork *) data)->fail = true;
    });
    SocketWork testTestWork(&testRequest, &testNameCallback, &cm, true);

    TestReportOutputRequest testReportOutputRequest(problemId);
    Callback testOutputCallback(this, [](void *data, stringstream &ss) {}, [](void *data) {
        ((TestWork *) data)->fail = true;
    });
    SocketWork testReportWork(&testReportOutputRequest, &testOutputCallback, &cm, true);

    auto reportResult = [&]() {
        bool flag = false;
        if (codeRunningResult != JudgeResultEnum::Accept) {
            testReportOutputRequest.setFlag(false);
        } else {
            testReportOutputRequest.setTestName(testName);

            int pos = 0;
            char *buffer = new char[1024];
            int fid = open(testOutPath.c_str(), O_RDONLY);
            if (fid == -1) {
                testReportOutputRequest.setFlag(false);
            } else {
                if (Env::ctx()->getBool(constant.localStorage)) {
                    return;
                }
                testReportOutputRequest.setFlag(true);
                while (read(fid, buffer, 1024) > 0) {
                    testReportOutputRequest.setPos(pos);
                    testReportOutputRequest.setValue(buffer);
                    cm.reset(1);
                    sessionPool->submit(&testReportWork);
                    cm.wait();
                    flag = true;
                }
            }
            if (!flag) {
                cm.reset(1);
                sessionPool->submit(&testReportWork);
                cm.wait();
            }
        }

        cm.reset(1);
        sessionPool->submit(&testReportWork);
        cm.wait();
    };

    for (int i = 0; i < testNum; ++i) {
        cm.reset(1);
        sessionPool->submit(&testTestWork);
        cm.wait();
        if (fail) {
            break;
        }

        /// region 获取输入输出数据

        cm.reset(1);
        Task *testInTask = new Task(this, [](void *data) {
            auto judgeWork = (TestWork *) data;
            judgeWork->testInPath = FileManager::checkTestDataIn(judgeWork->problemId, judgeWork->testName);
            judgeWork->testOutPath = FileManager::getTestDataOutPath(judgeWork->problemId, judgeWork->testName);
        }, &cm);
        threadPool->submit(testInTask);
        cm.wait();

        /// endregion

        if ((pipes[STD::input][0] = open(testInPath.c_str(), O_RDONLY)) == -1
            || (pipes[STD::output][1] = open(testOutPath.c_str(), O_WRONLY)) == -1
            || pipe(pipes[STD::codeError]) == -1) {

            for (auto &pipe: pipes) {
                for (int &pid: pipe) {
                    if (pid != -1) {
                        close(pid);
                        pid = -1;
                    }
                }
            }

            codeRunningResult = JudgeResultEnum::SystemError;
            reportResult();
            return;
        }

        cm.reset(1);
        Task *codeRunTask = new Task(this, [](void *data) {
            auto testWork = (TestWork *) data;
            testWork->codeRunningResult = testWork->runner->run(testWork->codePath,
                                                                testWork->pipes[STD::input],
                                                                testWork->pipes[STD::output],
                                                                testWork->pipes[STD::codeError],
                                                                testWork->timeLimit,
                                                                testWork->memoryLimit,
                                                                "",
                                                                nullptr,
                                                                false
            );
        }, &cm);
        threadPool->submit(codeRunTask);
        cm.wait();


        reportResult();
        for (auto &pipe: pipes) {
            for (int &pid: pipe) {
                if (pid != -1) {
                    close(pid);
                    pid = -1;
                }
            }
        }

        if (fail) {
            break;
        }
    }

    if (!fail) {
        TestReportOver testReportOver(problemId);
        Callback testCallback(this,
                              [](void *data, stringstream &ss) {},
                              [](void *data) {});
        SocketWork *testOverWork = new SocketWork(&testReportOver, &testCallback, &cm);
        cm.reset(1);
        sessionPool->submit(testOverWork);
        cm.wait();
    }
}

void TestWork::clean() const {
    FileManager::cleanTest(problemId);
}

TestWork::TestWork(id problemId, ThreadPool *tp, SessionPool *sp)
        : fail(false), codePath(), testName(), codeRunningResult(JudgeResultEnum::Accept),
          cm(1), language(nullptr), compiler(nullptr), runner(nullptr),
          problemId(problemId), testNum(0), timeLimit(0), memoryLimit(0),
          testInPath(), testOutPath(),
          threadPool(tp), sessionPool(sp) {
}

void TestWork::start() {
    if (init()) {
        run();
    }
    clean();
}

/// endregion

#endif //JUDGE_TEST_WORK_H
