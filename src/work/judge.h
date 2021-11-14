//
// Created by keqing on 2021-11-14.
//

#ifndef JUDGE_JUDGE_H
#define JUDGE_JUDGE_H

#include "../thread/__init__.h"
#include "../file/__init__.h"
#include "../compiler/__init__.h"
#include "../runner/__init__.h"
#include "../socket/__init__.h"

class JudgeWork {
private:
    bool fail;
    CountMutex cm;

    id solutionId{};
    id problemId{};
    Language *language{};
    string judgeName;
    int testNum{};
    unsigned long timeLimit{};
    unsigned long memoryLimit{};

    SessionPool *sessionPool;
    ThreadPool *threadPool;

    bool init();

public:

    JudgeWork(id solutionId, SessionPool *sp, ThreadPool *tp);
};

/// region define

bool JudgeWork::init() {
    SolutionInfoRequest request(solutionId);
    Callback callback(this, [&](void *data, stringstream &ss) {
        auto judge = (JudgeWork *) data;
        string name, value;
        while (ss >> name >> value) {
            if (name == "problemId") {
                problemId = toUL(value);
            } else if (name == "language") {
                judge->language = Language::getLanguage(value);
            } else if (name == "judgeName") {
                judgeName.swap(value)
            } else if (name == "testNum") {
                testNum = toInt(value);
            } else if (name == "timeLimit") {
                timeLimit = toUL(value);
            } else if (name == "memoryLimit") {
                memoryLimit = toUL(value);
            }
        }
    });

    auto socketWork = new SocketWork(&request, &callback, &cm);

    cm.reset(1);
    sessionPool->submit(socketWork);
    cm.wait();

    if (language == nullptr) return false;
    return true;
}

JudgeWork::JudgeWork(id solutionId, SessionPool *sp, ThreadPool *tp) : Task((void *) solutionId, [](void *data) {
    solutionId = (id) data;

    auto report = [&](const SolutionReportRequest &request, function<void(JudgeWork *, stringstream &)> &func) {
        request.setSolutionId(solutionId);
        Callback callback(this, [&](void *data, stringstream &ss) {
            func((JudgeWork *) data, ss);
        });
        auto socketWork = new SocketWork(&request, &callback, &cm);

        cm.reset(1);
        sessionPool->submit(socketWork);
        cm.wait();
    };

    auto reportNoCallback = [&](const SolutionReportRequest &request) {
        report(request, [](JudgeWork *, stringstream &) {});
    };

    auto reportResult = [&](JudgeResultEnum result) {
        SolutionReportRequest request("result", to_string((int) result));
        reportNoCallback(request);
    };


    if (!init()) {
        reportResult(JudgeResultEnum::SystemError);
        return;
    }

    FileManager::checkProblemDir(problemId);
    FileManager::createSolutionDir(solutionId);

    Compiler *compiler = CompilerFactory::getCompiler(*language);

    path judgePath, codePath;
    bool judgeCompilerResult, codeCompilerResult;
    auto judgeTask = new Task(nullptr, [&](void *) {
        judgePath = FileManager::checkJudge(judgeName, problemId, judgeCompilerResult);
    }, &cm);
    auto codeTask = new Task(nullptr, [&](void *) {
        codePath = FileManager::createSolutionCode(solutionId, *language);
        compilerResult = compiler->compile(codePath, language->getParams());
    }, &cm);

    cm.reset(2);
    threadPool->submit(judgeTask);
    threadPool->submit(codeTask);
    cm.wait();

    /// region 检查编译信息

    {
        if (!judgeCompilerResult) {
            reportResult(JudgeResultEnum::JudgeCompileError);
            return;
        }

        {
            string compileInfo;
            compiler->collectCompileInfo(codePath, compileInfo);
            SolutionReportRequest request("compile", compileInfo);
            reportNoCallback(request);
        }

        if (!codeCompilerResult) {
            reportResult(JudgeResultEnum::CompileError);
            return;
        }
    }

    /// endregion

    /// region 运行程序

    {
        Runner *judgeRunner = RunnerFactory::getRunner(Judge);
        Runner *codeRunner = RunnerFactory::getRunner(language);


        for (int i = 0; i < testNum; ++i) {
            SolutionReportRequest testNameRequest("testName", to_string(i));
            string testName;
            report(request, [&](JudgeWork *data, stringstream &ss) {
                ss >> testName;
            });
            path testInPath = FileManager::checkTestDataIn(problemId, testName);
            path testOutPath = FileManager::checkTestDataOut(problemId, testName);

            int pipes[4][2];
            memset(pipes, -1, sizeof(pipes));

            const static int codeInput = 0;
            const static int codeOutput = 1;
            const static int codeError = 2;
            const static int judgeError = 3;

            if ((pipes[codeInput][0] = open(testInPath.c_str(), O_RDONLY)) == -1
                || pipe(pipes[codeOutput]) == -1
                || pipe(pipes[codeError]) == -1
                || pipe(pipes[judgeError]) == -1) {

                for (int x = 0; x < 4; ++x)
                    for (int y = 0; y < 2; ++y)
                        if (pipes[x][y] != -1) close(pipes[x][y]);

                reportResult(JudgeResultEnum::SystemError);
                return;
            }

            JudgeResultEnum judgeResult, codeResult;
            Report codeReport;

            auto judgeRunnerTask = new Task(nullptr, [&](void *) {
                judgeResult = judgeRunner->run(judgePath, pipes[codeOutput], nullptr, pipes[judgeError], timeLimit,
                                               memoryLimit, testInPath.string() + "#" + testOutPath.string(), nullptr,
                                               true);
            }, &cm);
            auto codeRunnerTask = new Task(nullptr, [&](void *) {
                codeResult = judgeRunner->run(codePath, pipes[codeInput], pipes[codeOutput], pipes[codeError],
                                              timeLimit, memoryLimit, "", &codeReport, false);
            }, &cm);

            cm.reset(2);
            threadPool->submit(judgeRunnerTask);
            threadPool->submit(codeRunnerTask);
            cm.wait();

            for (int x = 0; x < 4; ++x)
                for (int y = 0; y < 2; ++y)
                    if (pipes[x][y] != -1) close(pipes[x][y]);

            if (codeResult == JudgeResultEnum::Accept) {
                if (judgeResult == JudgeResultEnum::WrongAnswer) {
                    codeResult = JudgeResultEnum::WrongAnswer;
                } else if (judgeCompilerResult != JudgeResultEnum::Accept) {
                    codeResult = JudgeResultEnum::JudgeFail;
                }
            }

            string response;
            SolutionReportRequest request("test", to_string((int) codeResult));
            report(request, [&](JudgeWork *judgeWork, stringstream &ss) {
                ss >> response;
            });

            if (response == "STOP") {
                break;
            }
        }
    }

    /// endregion

}), sessionPool(sp), threadPool(tp), fail(false), cm(0) {
}

/// endregion

#endif //JUDGE_JUDGE_H
