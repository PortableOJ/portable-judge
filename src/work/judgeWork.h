//
// Created by 胡柯青 on 2021-11-14.
//

#ifndef JUDGE_JUDGE_WORK_H
#define JUDGE_JUDGE_WORK_H

#include "../thread/__init__.h"
#include "../file/__init__.h"
#include "../compiler/__init__.h"
#include "../runner/__init__.h"
#include "../socket/__init__.h"

class JudgeWork {
private:
    JudgeResultEnum resultEnum;
    bool codeCompileSuccess;
    bool judgeCompileSuccess;
    JudgeResultEnum codeRunningResult;
    JudgeResultEnum judgeRunningResult;
    Report report;
    bool stopJudge;

    CountMutex cm;

    id solutionId;
    id problemId;

    const Language *language;
    const Compiler *compiler;
    const Runner *codeRunner;
    const Runner *judgeRunner;

    string judgeName;
    int testNum;
    unsigned long timeLimit;
    unsigned long memoryLimit;

    path codePath;
    path judgePath;

    string testName;
    path testInPath;

    SessionPool *sessionPool;

    int pipes[4][2]{};

    bool init();

    void run();

    void clean() const;

public:

    JudgeWork(id solutionId, SessionPool *sp);

    void start();
};

/// region define

bool JudgeWork::init() {
    Logger::trace("start for solution: %", solutionId);
    SolutionInfoRequest solutionInfoRequest(solutionId);
    Callback callback(this, [](void *data, stringstream &ss) {
        auto judgeWork = (JudgeWork *) data;
        string key;
        while (ss >> key) {
            if (key == constant.problemId) {
                ss >> judgeWork->problemId;
            } else if (key == constant.language) {
                string tmp;
                ss >> tmp;
                judgeWork->language = Language::getLanguage(tmp);
                if (judgeWork->language == nullptr) {
                    judgeWork->resultEnum = JudgeResultEnum::SystemError;
                }
            } else if (key == constant.judgeName) {
                ss >> judgeWork->judgeName;
            } else if (key == constant.testNum) {
                ss >> judgeWork->testNum;
            } else if (key == constant.timeLimit) {
                ss >> judgeWork->timeLimit;
            } else if (key == constant.memoryLimit) {
                ss >> judgeWork->memoryLimit;
                judgeWork->memoryLimit *= STD::MB;
            }
        }
    }, [](void *data) {
        auto judgeWork = (JudgeWork *) data;
        judgeWork->resultEnum = JudgeResultEnum::SystemError;
    });
    auto socketWork = new SocketWork(&solutionInfoRequest, &callback, &cm);
    sessionPool->submit(socketWork);
    cm.wait();
    if (resultEnum != JudgeResultEnum::Accept) {
        return false;
    }
    FileManager::createSolutionDir(solutionId);
    FileManager::checkProblemDir(problemId);
    return true;
}

void JudgeWork::run() {

    /// region 编译

    // 获取用户提交和编译用户提交
    codePath = FileManager::createSolutionCode(solutionId, *language);
    compiler = CompilerFactory::getCompiler(*language);
    codeCompileSuccess = compiler->compile(codePath, language->getParams());
    judgePath = FileManager::checkJudge(judgeName, problemId,judgeCompileSuccess);

    auto compileMsg = new string;
    compiler->collectCompileInfo(codePath, *compileMsg);
    auto solutionCompileMsgReportRequest = new SolutionCompileMsgReportRequest(
            solutionId, codeCompileSuccess, judgeCompileSuccess, *compileMsg);
    auto compileMsgCallback = new Callback(nullptr, [&](void *data, stringstream &ss) {}, [](void *data) {
        ((JudgeWork *) data)->stopJudge = true;
    });
    auto socketWork = new SocketWork(solutionCompileMsgReportRequest, compileMsgCallback, &cm);
    sessionPool->submit(socketWork);
    cm.wait();

    delete compileMsg;
    delete solutionCompileMsgReportRequest;
    delete compileMsgCallback;
    if (!codeCompileSuccess || !judgeCompileSuccess || stopJudge) {
        return;
    }

    /// endregion

    /// region 运行常用变量

    codePath.replace_extension(language->getRunningExtension());
    judgePath.replace_extension(Judge.getRunningExtension());

    codeRunner = RunnerFactory::getRunner(*language);
    judgeRunner = RunnerFactory::getRunner(Judge);

    SolutionTestRequest solutionTestRequest(solutionId);
    Callback solutionTestCallback(this, [](void *data, stringstream &ss) {
        ss >> ((JudgeWork *) data)->testName;
    }, [](void *data) {
        ((JudgeWork *) data)->stopJudge = true;
    });
    SocketWork solutionTestWork(&solutionTestRequest, &solutionTestCallback, &cm, true);

    SolutionTestReportRequest solutionTestReportRequest;
    Callback reportCallback(this, [](void *data, stringstream &ss) {}, [](void *data) {
        ((JudgeWork *) data)->stopJudge = true;
    });

    SocketWork reportSocketWork(&solutionTestReportRequest, &reportCallback, &cm, true);

    auto reportResult = [&](int pipeId) {
        solutionTestReportRequest.setSolutionId(solutionId);
        solutionTestReportRequest.setTestName(testName);
        solutionTestReportRequest.setTimeCost(report.timeCost);
        solutionTestReportRequest.setMemoryCost(report.memoryCost);
        if (resultEnum != JudgeResultEnum::Accept) {
            solutionTestReportRequest.setValue(static_cast<int>(resultEnum));
        } else if (judgeRunningResult != JudgeResultEnum::Accept) {
            solutionTestReportRequest.setValue(static_cast<int>(JudgeResultEnum::JudgeFail));
            Logger::trace("Judge fail, judgeResult: %", (int) judgeRunningResult);
        } else if (codeRunningResult != JudgeResultEnum::Accept) {
            solutionTestReportRequest.setValue(static_cast<int>(codeRunningResult));
        } else if (pipeId != -1) {
            int code = readInt(pipeId);
            int len = readInt(pipeId);
            string msg;
            msg.resize(len);
            read(pipeId, msg.data(), len);

            // testlib 输出 0 表示 AC，1 表示 WA，2 表示 Judge Fail
            switch (code) {
                case 0:
                    solutionTestReportRequest.setValue(static_cast<int>(JudgeResultEnum::Accept));
                    break;
                case 1:
                    solutionTestReportRequest.setValue(static_cast<int>(JudgeResultEnum::WrongAnswer));
                    break;
                case 2:
                default:
                    solutionTestReportRequest.setValue(static_cast<int>(JudgeResultEnum::JudgeFail));
                    break;
            }

            solutionTestReportRequest.setMsg(msg);
        } else {
            solutionTestReportRequest.setValue(static_cast<int>(JudgeResultEnum::SystemError));
        }

        cm.reset(1);
        sessionPool->submit(&reportSocketWork);
        cm.wait();
    };

    // 初始化所有的 pipe
    for (auto &pipe: pipes) {
        for (int &pid: pipe) {
            pid = -1;
        }
    }

    /// endregion

    for (int i = 0; i < testNum; ++i) {
        cm.reset(1);
        sessionPool->submit(&solutionTestWork);
        cm.wait();
        if (stopJudge) {
            break;
        }

        /// region 获取输入输出数据

        testInPath = FileManager::checkTestDataIn(problemId, testName);
        FileManager::checkTestDataOut(problemId, testName);

        /// endregion

        /// region 运行

        if ((pipes[STD::input][0] = open(testInPath.c_str(), O_RDONLY)) == -1
            || pipe(pipes[STD::output]) == -1
            || pipe(pipes[STD::codeError]) == -1
            || pipe(pipes[STD::judgeError]) == -1) {

            for (auto &pipe: pipes) {
                for (int &pid: pipe) {
                    if (pid != -1) {
                        close(pid);
                        pid = -1;
                    }
                }
            }

            resultEnum = JudgeResultEnum::SystemError;
            reportResult(pipes[STD::judgeError][0]);
            return;
        }

        int codePid = codeRunner->run(codePath,
                                      pipes[STD::input], pipes[STD::output], pipes[STD::codeError],
                                      timeLimit, memoryLimit,
                                      "", false);

        int judgePid = judgeRunner->run(judgePath,
                                    pipes[STD::output], nullptr, pipes[STD::judgeError],
                                    timeLimit, memoryLimit,
                                    testInPath.string(), true);

        codeRunningResult = Runner::trace(codePid, pipes[STD::codeError][0], &report);
        judgeRunningResult = Runner::trace(codePid, pipes[STD::judgeError][0], nullptr);

        /// endregion

        reportResult(pipes[STD::judgeError][0]);

        if (pipes[STD::codeError][0] != -1) {
            close(pipes[STD::codeError][0]);
        }

        if (pipes[STD::judgeError][0] != -1) {
            close(pipes[STD::judgeError][0]);
        }

        Logger::trace("Finish for solution: %, stopJudge: %", solutionId, stopJudge);
        if (stopJudge) {
            break;
        }
    }
}

void JudgeWork::clean() const {
    FileManager::cleanSolution(solutionId);
}

JudgeWork::JudgeWork(id solutionId, SessionPool *sp)
        : resultEnum(JudgeResultEnum::Accept), codeCompileSuccess(false), judgeCompileSuccess(false),
          codeRunningResult(JudgeResultEnum::Accept), judgeRunningResult(JudgeResultEnum::Accept),
          report(), stopJudge(false), cm(1), solutionId(solutionId), problemId(0),
          language(nullptr), compiler(nullptr), codeRunner(nullptr), judgeRunner(nullptr),
          judgeName(), testNum(0), timeLimit(0), memoryLimit(0), codePath(), judgePath(),
          testName(), testInPath(), sessionPool(sp) {}

void JudgeWork::start() {
    if (init()) {
        run();
    }
    clean();
}

/// endregion

#endif //JUDGE_JUDGE_WORK_H
