//
// Created by 胡柯青 on 2021-11-07.
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
    static path judgePath;
    static path testPath;

    static SessionPool *sessionPool;
    static ThreadPool *threadPool;
    const static Compiler *cppCompiler;

    /**
     * 文件系统全局锁
     */
    static Mutex<char> fileMutex;

    static bool initStandardJudge();

public:

    static void setHome();

    static bool init(SessionPool *sp, ThreadPool *tp);

    static void checkProblemDir(id problemId);

    static void createSolutionDir(id solutionId);

    static path createSolutionCode(id solutionId, const Language &language);

    static void createTestDir(id problemId);

    static path createTestCode(id problemId, const Language &language);

    static path checkJudge(const string &judgeName, id problemId, bool &compileResult);

    static path checkTestDataIn(id problemId, const string &name);

    static path checkTestDataOut(id problemId, const string &name);

    static path getTestDataOutPath(id problemId, const string &name);

    static void cleanSolution(id solutionId);

    static void cleanTest(id problemId);

    static void cleanProblem(id problemId);

    static void cleanProblemJudge(id problemId);
};

/// region define

const string FileManager::defaultHome = "/portable";                 // NOLINT

bool FileManager::localStorage = false;

path FileManager::problemPath("problem");                   // NOLINT
path FileManager::solutionPath("solution");                 // NOLINT
path FileManager::judgePath("judge");                       // NOLINT
path FileManager::testPath("test");                       // NOLINT

SessionPool *FileManager::sessionPool = nullptr;
ThreadPool *FileManager::threadPool = nullptr;
const Compiler *FileManager::cppCompiler = nullptr;

Mutex<char> FileManager::fileMutex;

bool FileManager::initStandardJudge() {
    CountMutex cm(0);

    /// region 获取 test lib

    cm.reset(1);
    auto testLibRequest = new TestLibRequest();
    path testLibPath = judgePath / constant.testlib;
    auto testLibCallback = new Callback(testLibPath);
    Job *getTestLibCode = new SocketWork(testLibRequest, testLibCallback, &cm);
    sessionPool->submit(getTestLibCode);
    cm.wait();

    /// endregion

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
    int len = (int) standardJudgeNameList.size();

    /// endregion

    /// region 保存标准程序文件

    vector<path> standardJudgePathList(len);
    cm.reset(len);
    vector<StandardJudgeCodeRequest *> requestList(len);
    vector<Callback *> callbackList(len);

    for (int i = 0; i < len; ++i) {
        const string &fileName = standardJudgeNameList[i];
        requestList[i] = new StandardJudgeCodeRequest(fileName);
        standardJudgePathList[i] = judgePath / fileName;
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

void FileManager::setHome() {
    const string *home = Env::ctx()->getString(constant.home);

    if (home == nullptr) {
        create_directories(defaultHome);
        chdir(defaultHome.c_str());
    } else {
        create_directories(*home);
        chdir(home->c_str());
    }

}

bool FileManager::init(SessionPool *sp, ThreadPool *tp) {
    create_directories(problemPath);
    create_directories(solutionPath);
    create_directories(judgePath);
    create_directories(testPath);

    sessionPool = sp;
    threadPool = tp;
    cppCompiler = CompilerFactory::getCompiler(Judge);

    localStorage = Env::ctx()->getBool(constant.localStorage);

    return initStandardJudge();
}

void FileManager::checkProblemDir(id problemId) {
    path curProblemPath = problemPath / to_string(problemId);
    if (!exists(curProblemPath)) {
        create_directories(curProblemPath);
    }
}

void FileManager::createSolutionDir(id solutionId) {
    path curSolutionPath = solutionPath / to_string(solutionId);
    if (!exists(curSolutionPath)) {
        create_directories(curSolutionPath);
    }
}

path FileManager::createSolutionCode(id solutionId, const Language &language) {
    path res = solutionPath / to_string(solutionId) / language.getCodeName();
    res += language.getExtension();

    SolutionCodeRequest request(solutionId);
    Callback callback(res.string(), [](void *data) {});

    CountMutex cm(1);
    auto socketWork = new SocketWork(&request, &callback, &cm);
    sessionPool->submit(socketWork);
    cm.wait();

    return res;
}

void FileManager::createTestDir(id problemId) {
    path curTestPath = testPath / to_string(problemId);
    if (!exists(curTestPath)) {
        create_directories(curTestPath);
    }
}

path FileManager::createTestCode(id problemId, const Language &language) {
    path res = testPath / to_string(problemId) / language.getCodeName();
    res += language.getExtension();

    TestStdCodeRequest request(problemId);
    Callback callback(res.string(), [](void *data) {});

    CountMutex cm(1);
    auto socketWork = new SocketWork(&request, &callback, &cm);
    sessionPool->submit(socketWork);
    cm.wait();

    return res;
}

path FileManager::checkJudge(const string &judgeName, id problemId, bool &compileResult) {
    if (judgeName == constant.useDiyJudge) {
        compileResult = true;
        path curJudgePath = judgePath / to_string(problemId);
        curJudgePath.replace_extension(Judge.getRunningExtension());
        fileMutex.run([&](char &) {
            if (exists(curJudgePath)) return;
            curJudgePath.replace_extension(Judge.getExtension());
            CountMutex cm(1);
            ProblemJudgeCodeRequest request(problemId);
            Callback callback(curJudgePath, [](void *data) {});
            auto socketWork = new SocketWork(&request, &callback, &cm);
            sessionPool->submit(socketWork);
            cm.wait();
            compileResult = cppCompiler->compile(curJudgePath, Judge.getParams());
        });
        return curJudgePath;
    } else {
        path curJudgePath = judgePath / judgeName;
        compileResult = true;
        return curJudgePath;
    }
}

path FileManager::checkTestDataIn(id problemId, const string &name) {
    path dataIn = problemPath / to_string(problemId) / name;
    dataIn += ".in";
    fileMutex.run([&](char &){
        if (exists(dataIn)) return;

        CountMutex cm(1);
        ProblemDataInRequest request(problemId, name);
        Callback callback(dataIn);
        auto socketWork = new SocketWork(&request, &callback, &cm);
        sessionPool->submit(socketWork);
        cm.wait();
    });

    return dataIn;
}

path FileManager::checkTestDataOut(id problemId, const string &name) {
    path dataOut = getTestDataOutPath(problemId, name);
    fileMutex.run([&](char &) {
        if (exists(dataOut)) return;

        CountMutex cm(1);
        ProblemDataOutRequest request(problemId, name);
        Callback callback(dataOut);
        auto socketWork = new SocketWork(&request, &callback, &cm);
        sessionPool->submit(socketWork);
        cm.wait();
    });

    return dataOut;
}

path FileManager::getTestDataOutPath(id problemId, const string &name) {
    path dataOut = problemPath / to_string(problemId) / name;
    dataOut += ".out";
    return dataOut;
}

void FileManager::cleanSolution(id solutionId) {
    path solution = solutionPath / to_string(solutionId);
    remove_all(solution);
}

void FileManager::cleanTest(id problemId) {
    path test = testPath / to_string(problemId);
    remove_all(test);
}

void FileManager::cleanProblem(id problemId) {
    if (localStorage) {
        Logger::info("stop clean problem %, because of local", problemId);
        return;
    }
    path problem = problemPath / to_string(problemId);
    remove_all(problem);
}

void FileManager::cleanProblemJudge(id problemId) {
    path curJudgePath = judgePath / to_string(problemId);
    remove(curJudgePath);
}

/// endregion

#endif //JUDGE_FILE_MANAGER_H
