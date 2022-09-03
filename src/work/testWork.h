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

    bool init();

    void run();

    void clean() const;

public:
    TestWork(id problemId, SessionPool *sp);

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
                if (testWork->language == nullptr) {
                    testWork->fail = true;
                }
            } else if (key == constant.testNum) {
                ss >> testWork->testNum;
            } else if (key == constant.timeLimit) {
                ss >> testWork->timeLimit;
            } else if (key == constant.memoryLimit) {
                ss >> testWork->memoryLimit;
                testWork->memoryLimit *= STD::MB;
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
    FileManager::checkProblemDir(problemId);
    return true;
}

void TestWork::run() {

    /// region 编译

    Logger::debug("Start compile");
    codePath = FileManager::createTestCode(problemId, *language);
    compiler = CompilerFactory::getCompiler(*language);
    runner = RunnerFactory::getRunner(*language);
    fail = !compiler->compile(codePath, language->getParams());
    if (fail) {
        TestCompileReportRequest testCompileReportRequest(problemId);
        Callback compileMsgCallback(this, [](void *data, stringstream &ss) {}, [](void *data) {
            ((TestWork *) data)->fail = true;
        });
        cm.reset(1);
        auto socketWork = new SocketWork(&testCompileReportRequest, &compileMsgCallback, &cm);
        sessionPool->submit(socketWork);
        cm.wait();
        return;
    }
    Logger::debug("End compile & Start run");

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
        testReportOutputRequest.setTestName(testName);
        if (codeRunningResult != JudgeResultEnum::Accept) {
            testReportOutputRequest.setFlag(false);
            cm.reset(1);
            sessionPool->submit(&testReportWork);
            cm.wait();
        } else {
            int pos = 0;
            char *buffer = new char[1024];
            int fid = open(testOutPath.c_str(), O_RDONLY);
            if (fid == -1) {
                testReportOutputRequest.setFlag(false);
                cm.reset(1);
                sessionPool->submit(&testReportWork);
                cm.wait();
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
                    pos++;
                }
                close(fid);
                fid = -1;
            }
        }
    };

    // 初始化所有的 pipe
    for (auto &pipe: pipes) {
        for (int &pid: pipe) {
            pid = -1;
        }
    }

    for (int i = 0; i < testNum; ++i) {
        Logger::debug("Start run for test: %", i);

        cm.reset(1);
        sessionPool->submit(&testTestWork);
        cm.wait();
        if (fail) {
            break;
        }

        /// region 获取输入输出数据

        testInPath = FileManager::checkTestDataIn(problemId, testName);
        testOutPath = FileManager::getTestDataOutPath(problemId, testName);

        /// endregion

        if ((pipes[STD::input][0] = open(testInPath.c_str(), O_RDONLY)) == -1
            || (pipes[STD::output][1] = open(testOutPath.c_str(), O_WRONLY | O_CREAT, 0666)) == -1
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

        codePath.replace_extension(language->getRunningExtension());

        int runPid = runner->run(codePath,
                    pipes[STD::input], pipes[STD::output], pipes[STD::codeError],
                    timeLimit, memoryLimit,
                    "", false);

        codeRunningResult = Runner::trace(runPid, pipes[STD::codeError][0], nullptr);

        reportResult();

        if (pipes[STD::codeError][0] != -1) {
            close(pipes[STD::codeError][0]);
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
        auto *testOverWork = new SocketWork(&testReportOver, &testCallback, &cm);
        cm.reset(1);
        sessionPool->submit(testOverWork);
        cm.wait();
    }

    Logger::debug("end test work");
}

void TestWork::clean() const {
    FileManager::cleanTest(problemId);
}

TestWork::TestWork(id problemId, SessionPool *sp)
        : fail(false), codePath(), testName(), codeRunningResult(JudgeResultEnum::Accept),
          cm(1), language(nullptr), compiler(nullptr), runner(nullptr),
          problemId(problemId), testNum(0), timeLimit(0), memoryLimit(0),
          testInPath(), testOutPath(), sessionPool(sp) {
}

void TestWork::start() {
    if (init()) {
        run();
    }
    clean();
}
/// endregion

#endif //JUDGE_TEST_WORK_H
