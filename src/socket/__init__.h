//
// Created by 胡柯青 on 2021-10-22.
//

#ifndef JUDGE_SOCKET_INIT_H
#define JUDGE_SOCKET_INIT_H

#include "model/appendRequest.h"
#include "model/heartbeatRequest.h"
#include "model/problemDataInRequest.h"
#include "model/problemDataOutRequest.h"
#include "model/problemJudgeCodeRequest.h"
#include "model/registerRequest.h"
#include "model/solutionCodeRequest.h"
#include "model/solutionCompileMsgReportRequest.h"
#include "model/solutionInfoRequest.h"
#include "model/solutionTestReportRequest.h"
#include "model/solutionTestRequest.h"
#include "model/standardJudgeCodeRequest.h"
#include "model/standardJudgeRequest.h"
#include "model/testInfoRequest.h"
#include "model/testLibRequest.h"
#include "model/testReportOutputRequest.h"
#include "model/testCompileReportRequest.h"
#include "model/testStdCodeRequest.h"
#include "model/testReportOver.h"
#include "model/testTestRequest.h"

#include "callback.h"
#include "request.h"
#include "session.h"
#include "sessionPool.h"
#include "socketWork.h"

#endif //JUDGE_SOCKET_INIT_H
