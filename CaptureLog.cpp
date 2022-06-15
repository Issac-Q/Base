#include "CaptureLog.h"
#include <stdio.h>
#include <map>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/time.h>
#include <algorithm>
#include <stdarg.h>
#include <string.h>

#define USE_PRINTF

#define USE_CALLDEPTH

static const int LOG_SINGLE_LOG_SIZE = 1024;
static const char* const LOG_FOMAT_STR = "[%s]tid:%5d[%s::%s:%d]%s%s";
static const char* const LOG_FOMAT_STR_PERF = "%s#<%s>#%ld#tid:%5d #%s";


int GetThreadId()
{
    return syscall(__NR_gettid);
}

unsigned long GetTickCount()
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec * 1000 + time.tv_nsec / 1000000;
}

unsigned long GetThreadTime()
{
    struct timespec time;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time);
    return time.tv_sec * 1000 + time.tv_nsec / 1000000;
}

struct callinfo{
    int depth;
    char mark;
};
const static char s_defMark = 'x';
const static int s_maxdepth = 32;
static std::map<int, callinfo> s_mapTid2Depth;

inline const char* GetCallDepthStr() {
    static char retStr[s_maxdepth + 2] = {0};

#ifdef USE_CALLDEPTH
    int tid = GetThreadId();
    callinfo info = {0, s_defMark};
    if (s_mapTid2Depth.count(tid)) {
    info = s_mapTid2Depth[tid];
    }

    memset(retStr, info.mark, info.depth);
    retStr[info.depth] = ' ';
    retStr[info.depth+1] = '\0';

#endif
    return retStr;
}

int CaptureLog::CaptureLog_Debug = 1;
int CaptureLog::CaptureLog_Info = 2;
int CaptureLog::CaptureLog_Warn = 3;
int CaptureLog::CaptureLog_Error = 4;
int CaptureLog::CaptureLog_Performace = 5;
int CaptureLog::CaptureLog_LostFrame = 6;

std::vector<int> CaptureLog::m_logLevels;

void CaptureLog::setLogLevel(const std::string& levels)
{
    if (levels.find('N') != std::string::npos) {
        return;
    }
    for (size_t i = 0; i < levels.size(); ++i) {
        if ('D' == levels[i]) {
            m_logLevels.push_back(CaptureLog_Debug);
        }
        else if ('I' == levels[i]) {
            m_logLevels.push_back(CaptureLog_Info);
        }
        else if ('W' == levels[i]) {
            m_logLevels.push_back(CaptureLog_Warn);
        }
        else if ('E' == levels[i]) {
            m_logLevels.push_back(CaptureLog_Error);
        }
        else {
            printf("[Error] log level error:%s\n", levels[i]);
        }
    }
}

void CaptureLog::output(const char *mainTag, const char *subTag, int type, const char *pszFileName, int iLineNum, const char *pszFuncName, const char *format, ...)
{
    if (m_logLevels.empty()) {
        return;
    }

    if (std::find(m_logLevels.begin(), m_logLevels.end(), type) == m_logLevels.end()) {
        // not found log tyoe
        return;
    }
    char szTempLogInfo[LOG_SINGLE_LOG_SIZE] = {0};
    va_list list;
    va_start(list, format); //"format"是最后一个固定参数，这个宏收集可变参数放在一个va_list里面
    vsnprintf(szTempLogInfo, LOG_SINGLE_LOG_SIZE, format, list);
    va_end(list);

    char szOutputInfo[LOG_SINGLE_LOG_SIZE] = {0};
    snprintf(szOutputInfo, LOG_SINGLE_LOG_SIZE, LOG_FOMAT_STR,
    subTag,
    GetThreadId(),
    strrchr(pszFileName, '/') + 1,
    pszFuncName,
    iLineNum,
    GetCallDepthStr(),
    szTempLogInfo);
#ifdef USE_PRINTF
    // check log level
    printf("%s\n", szOutputInfo);
#endif
}

void CaptureLog::output_s(const char *mainTag, const char *subTag, int type, const char *format, ...)
{
    char szTempLogInfo[LOG_SINGLE_LOG_SIZE] = {0};

    va_list list;
    va_start(list, format);
    vsnprintf(szTempLogInfo, LOG_SINGLE_LOG_SIZE, format, list);
    va_end(list);

    char szOutputInfo[LOG_SINGLE_LOG_SIZE] = {0};
    snprintf(szOutputInfo, LOG_SINGLE_LOG_SIZE, LOG_FOMAT_STR_PERF,
    subTag,
    GetTickCount(),
    GetThreadTime(),
    GetThreadId(),
    szTempLogInfo);
#ifdef USE_PRINTF
    printf("%s\n", szOutputInfo);
#endif
}
