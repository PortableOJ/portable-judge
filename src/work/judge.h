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

class Judge {
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

    Judge(id solutionId, SessionPool *sp, ThreadPool *tp);
};

/// region define

bool Judge::init() {
    SolutionInfoRequest request(solutionId);
    Callback callback(this, [&](void *data, stringstream &ss) {
        auto judge = (Judge *) data;
        string name, value;
        while (ss >> name >> value) {
            if (name == "problemId") {
                problemId = toUL(value);
            } else if (name == "language") {
                judge->language = Language::getLanguage(value);
            } else if (name == "judgeName") {
                judgeName = move(value);
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

Judge::Judge(id solutionId, SessionPool *sp, ThreadPool *tp) : Task((void *) solutionId, [](void *data) {
    solutionId = (id) data;
    if (!init()) {
        SolutionReportRequest request("init", "init Fail");
        Callback callback(this, [&](void *data, stringstream &ss) {});
        auto socketWork = new SocketWork(&request, &callback, &cm);

        cm.reset(1);
        sessionPool->submit(socketWork);
        cm.wait();
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

    /// region 提交代码编译信息

    {
        if (!judgeCompilerResult) {
            SolutionReportRequest request("result", to_string((int) JudgeResultEnum::JudgeCompileError));
        }

        // TODO

        string compileInfo;
        compiler->collectCompileInfo(codePath, compileInfo);

        Callback callback(this, [&](void *data, stringstream &ss) {});
        auto socketWork = new SocketWork(&request, &callback, &cm);
        cm.reset(1);
        sessionPool->submit(socketWork);
        cm.wait();

    }

    /// endregion


}), sessionPool(sp), threadPool(tp), fail(false), cm(0) {
}

/// endregion

#endif //JUDGE_JUDGE_H
