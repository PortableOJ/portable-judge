//
// Created by keqing on 2021-11-07.
//

#ifndef JUDGE_FILE_MANAGER_H
#define JUDGE_FILE_MANAGER_H

#include "../socket/__init__.h"
#include "../compiler/__init__.h"
#include "../language/__init__.h"

class FileManager {
private:
    const static string defaultHome;

    static bool localStorage;

    static path problemPath;
    static path solutionPath;
    static path standardJudgePath;

    static SessionPool *sessionPool;
    static ThreadPool *threadPool;
    const static Compiler *cppCompiler;

    static bool initStandardJudge();

public:
    static bool init(SessionPool *sp, ThreadPool *tp);

    static void checkProblemDir(int problemId);

    static path createSolutionDir(int solutionId);

    static path createSolutionCode(int solutionId, const Language &language);

    static path checkJudge(const string &judgeName, int problemId);

    static path checkTestDataIn(int problemId, const string &name);

    static path checkTestDataOut(int problemId, const string &name);

    static void cleanProblem(int problemId);
};

/// region define

const string FileManager::defaultHome = "/portable";                 // NOLINT

bool FileManager::localStorage = false;

path FileManager::problemPath("problem");                   // NOLINT
path FileManager::solutionPath("solution");                 // NOLINT
path FileManager::standardJudgePath("standardJudge");       // NOLINT

SessionPool *FileManager::sessionPool = nullptr;
ThreadPool *FileManager::threadPool = nullptr;
const Compiler *FileManager::cppCompiler = nullptr;

bool FileManager::initStandardJudge() {
    CountMutex cm(0);

    /// region 获取标准 judge 程序列表

    cm.reset(1);
    StandardJudgeRequest standardJudgeRequest;
    vector<string> standardJudgeNameList;
    Callback standardJudgeCallback(nullptr, [&](void *, stringstream &ss) {
        string name, param;
        while (ss >> name) {
            standardJudgeNameList.push_back(name);
        }
    });
    Job *getStandardJudgeList = new SocketWork(&standardJudgeRequest, &standardJudgeCallback, &cm);
    sessionPool->submit(getStandardJudgeList);
    cm.wait();
    int len = standardJudgeNameList.size();

    /// endregion

    /// region 保存标准程序文件

    vector<path> standardJudgePathList(len);
    cm.reset(len);
    vector<StandardJudgeCodeRequest *> requestList(len);
    vector<Callback *> callbackList(len);

    for (int i = 0; i < len; ++i) {
        const string &fileName = standardJudgeNameList[i];
        requestList[i] = new StandardJudgeCodeRequest(fileName);
        standardJudgePathList[i] = standardJudgePath / fileName;
        standardJudgePathList[i] += Judge.getExtension();
        callbackList[i] = new Callback(standardJudgePathList[i]);
        Job *getJudgeCode = new SocketWork(requestList[i], callbackList[i], &cm);
        sessionPool->submit(getJudgeCode);
    }

    cm.wait();

    for (int i = 0; i < len; ++i) {
        delete requestList[i];
        delete callbackList[i];
    }

    /// endregion

    /// region 编译标准程序

    Mutex<bool> mutex(true);

    cm.reset(len);
    for (long i = 0; i < len; ++i) {
        auto job = new Task((void *) i, [&](void *index) {
            bool isCompilerOk = cppCompiler->compile(standardJudgePathList[(long) index], Judge.getParams());
            mutex.run([&](bool &data) {
                data &= isCompilerOk;
            });
        }, &cm);
        threadPool->submit(job);
    }

    cm.wait();

    /// endregion

    return mutex.get();
}

bool FileManager::init(SessionPool *sp, ThreadPool *tp) {
    const string *home = Env::ctx()->getString(constant.home);

    if (home == nullptr) {
        create_directories(defaultHome);
        chdir(defaultHome.c_str());
    } else {
        create_directories(*home);
        chdir(home->c_str());
    }

    create_directories(problemPath);
    create_directories(solutionPath);
    create_directories(standardJudgePath);

    sessionPool = sp;
    threadPool = tp;
    cppCompiler = CompilerFactory::getCompiler(Judge);

    localStorage = Env::ctx()->getBool(constant.localStorage);

    return initStandardJudge();
}

void FileManager::checkProblemDir(int problemId) {
    path curProblemPath = problemPath / to_string(problemId);
    if (!exists(curProblemPath)) {
        create_directories(curProblemPath);
    }
}

path FileManager::createSolutionDir(int solutionId) {
    path curSolutionPath = solutionPath / to_string(solutionId);
    if (!exists(curSolutionPath)) {
        create_directories(curSolutionPath);
    }
    return curSolutionPath;
}

path FileManager::createSolutionCode(int solutionId, const Language &language) {
    path res = solutionPath / to_string(solutionId) / language.getCodeName();
    res += language.getExtension();

    SolutionCodeRequest request(solutionId);
    Callback callback(res.string());

    CountMutex cm(1);
    auto socketWork = new SocketWork(&request, &callback, &cm);
    sessionPool->submit(socketWork);
    cm.wait();

    return res;
}

path FileManager::checkJudge(const string &judgeName, int problemId) {
    if (judgeName == constant.useDiyJudge) {
        path judgePath = problemPath / to_string(problemId) / Judge.getCodeName();
        if (exists(judgePath)) return judgePath;

        judgePath += Judge.getExtension();
        CountMutex cm(1);
        ProblemJudgeCodeRequest request(problemId);
        Callback callback(judgePath);
        auto socketWork = new SocketWork(&request, &callback, &cm);
        sessionPool->submit(socketWork);
        cm.wait();

        cppCompiler->compile(judgePath, Judge.getParams());

        judgePath.replace_extension("");
        return judgePath;
    } else {
        path judgePath = standardJudgePath / judgeName;
        return judgePath;
    }
}

path FileManager::checkTestDataIn(int problemId, const string &name) {
    path dataIn = problemPath / to_string(problemId) / name;
    dataIn += ".in";
    if (exists(dataIn)) return dataIn;

    CountMutex cm(1);
    ProblemDataInRequest request(problemId, name);
    Callback callback(dataIn);
    auto socketWork = new SocketWork(&request, &callback, &cm);
    sessionPool->submit(socketWork);
    cm.wait();

    return dataIn;
}

path FileManager::checkTestDataOut(int problemId, const string &name) {
    path dataOut = problemPath / to_string(problemId) / name;
    dataOut += ".out";
    if (exists(dataOut)) return dataOut;

    CountMutex cm(1);
    ProblemDataOutRequest request(problemId, name);
    Callback callback(dataOut);
    auto socketWork = new SocketWork(&request, &callback, &cm);
    sessionPool->submit(socketWork);
    cm.wait();

    return dataOut;
}

void FileManager::cleanProblem(int problemId) {
    if (localStorage) return;
    path problem = problemPath / to_string(problemId);
    remove_all(problem);
}

/// endregion

#endif //JUDGE_FILE_MANAGER_H
