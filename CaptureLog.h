#ifndef CAPTURELOG_H
#define CAPTURELOG_H

#include <vector>
#include <string>


#ifndef CAPTURE_LOG
#define CAPTURE_LOG

#define CAPTURE_LOG_D(maintag, subtag, ...) ((void)CaptureLog::output(maintag, subtag, CaptureLog::CaptureLog_Debug, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__))
#define CAPTURE_LOG_I(maintag, subtag, ...) ((void)CaptureLog::output(maintag, subtag, CaptureLog::CaptureLog_Info, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__))
#define CAPTURE_LOG_W(maintag, subtag, ...) ((void)CaptureLog::output(maintag, subtag, CaptureLog::CaptureLog_Warn, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__))
#define CAPTURE_LOG_E(maintag, subtag, ...) ((void)CaptureLog::output(maintag, subtag, CaptureLog::CaptureLog_Error, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__))

#endif // CAPTURE_LOG


#ifndef LOG_JANUSLOG_MAINTAG
#define LOG_JANUSLOG_MAINTAG "ACQ"

#ifndef LOG_CAPTURE

#define LOG_CAPTURE_TAG "Capture "
#define LOG_CAPTURE_D(...) CAPTURE_LOG_D(LOG_JANUSLOG_MAINTAG, LOG_CAPTURE_TAG, __VA_ARGS__)
#define LOG_CAPTURE_I(...) CAPTURE_LOG_I(LOG_JANUSLOG_MAINTAG, LOG_CAPTURE_TAG, __VA_ARGS__)
#define LOG_CAPTURE_W(...) CAPTURE_LOG_W(LOG_JANUSLOG_MAINTAG, LOG_CAPTURE_TAG, __VA_ARGS__)
#define LOG_CAPTURE_E(...) CAPTURE_LOG_E(LOG_JANUSLOG_MAINTAG, LOG_CAPTURE_TAG, __VA_ARGS__)
#define LOG_CAPTURE(...)   CAPTURE_LOG_D(LOG_JANUSLOG_MAINTAG, LOG_CAPTURE_TAG, __VA_ARGS__)

#endif // LOG_CAPTURE

#ifndef LOG_REPLAY

#define LOG_REPLAY_TAG "Replay  "
#define LOG_REPLAY_D(...) CAPTURE_LOG_D(LOG_JANUSLOG_MAINTAG, LOG_REPLAY_TAG, __VA_ARGS__)
#define LOG_REPLAY_I(...) CAPTURE_LOG_I(LOG_JANUSLOG_MAINTAG, LOG_REPLAY_TAG, __VA_ARGS__)
#define LOG_REPLAY_W(...) CAPTURE_LOG_W(LOG_JANUSLOG_MAINTAG, LOG_REPLAY_TAG, __VA_ARGS__)
#define LOG_REPLAY_E(...) CAPTURE_LOG_E(LOG_JANUSLOG_MAINTAG, LOG_REPLAY_TAG, __VA_ARGS__)
#define LOG_REPLAY(...)   CAPTURE_LOG_D(LOG_JANUSLOG_MAINTAG, LOG_REPLAY_TAG, __VA_ARGS__)

#endif // LOG_REPLAY

#ifndef LOG_DATAENGINE

#define LOG_DATAENGINE_TAG "DataSrcE"
#define LOG_DATAENGINE_D(...) CAPTURE_LOG_D(LOG_JANUSLOG_MAINTAG, LOG_DATAENGINE_TAG, __VA_ARGS__)
#define LOG_DATAENGINE_I(...) CAPTURE_LOG_I(LOG_JANUSLOG_MAINTAG, LOG_DATAENGINE_TAG, __VA_ARGS__)
#define LOG_DATAENGINE_W(...) CAPTURE_LOG_W(LOG_JANUSLOG_MAINTAG, LOG_DATAENGINE_TAG, __VA_ARGS__)
#define LOG_DATAENGINE_E(...) CAPTURE_LOG_E(LOG_JANUSLOG_MAINTAG, LOG_DATAENGINE_TAG, __VA_ARGS__)
#define LOG_DATAENGINE(...)   CAPTURE_LOG_D(LOG_JANUSLOG_MAINTAG, LOG_DATAENGINE_TAG, __VA_ARGS__)

#endif // LOG_DATAENGINE

#ifndef LOG_CAN

#define LOG_CAN_TAG "CAN     "
#define LOG_CAN_D(...) CAPTURE_LOG_D(LOG_JANUSLOG_MAINTAG, LOG_CAN_TAG, __VA_ARGS__)
#define LOG_CAN_I(...) CAPTURE_LOG_I(LOG_JANUSLOG_MAINTAG, LOG_CAN_TAG, __VA_ARGS__)
#define LOG_CAN_W(...) CAPTURE_LOG_W(LOG_JANUSLOG_MAINTAG, LOG_CAN_TAG, __VA_ARGS__)
#define LOG_CAN_E(...) CAPTURE_LOG_E(LOG_JANUSLOG_MAINTAG, LOG_CAN_TAG, __VA_ARGS__)
#define LOG_CAN(...)   CAPTURE_LOG_D(LOG_JANUSLOG_MAINTAG, LOG_CAN_TAG, __VA_ARGS__)

#endif // LOG_CAN

#ifndef LOG_SOCKET

#define LOG_SOCKET_TAG "Socket  "
#define LOG_SOCKET_D(...) CAPTURE_LOG_D(LOG_JANUSLOG_MAINTAG, LOG_SOCKET_TAG, __VA_ARGS__)
#define LOG_SOCKET_I(...) CAPTURE_LOG_I(LOG_JANUSLOG_MAINTAG, LOG_SOCKET_TAG, __VA_ARGS__)
#define LOG_SOCKET_W(...) CAPTURE_LOG_W(LOG_JANUSLOG_MAINTAG, LOG_SOCKET_TAG, __VA_ARGS__)
#define LOG_SOCKET_E(...) CAPTURE_LOG_E(LOG_JANUSLOG_MAINTAG, LOG_SOCKET_TAG, __VA_ARGS__)
#define LOG_SOCKET(...)   CAPTURE_LOG_D(LOG_JANUSLOG_MAINTAG, LOG_SOCKET_TAG, __VA_ARGS__)

#endif // LOG_CAN

// perform log
#ifndef LOG_PERFORMACE

#define LOG_PERFORMACE(...) ((void)CaptureLog::output_s(LOG_JANUSLOG_MAINTAG, "Perf", CaptureLog::CaptureLog_Performace, __VA_ARGS__))

#endif // LOG_PERFORMACE
#endif

class CaptureLog
{
public:
    static int CaptureLog_Debug;
    static int CaptureLog_Info;
    static int CaptureLog_Warn;
    static int CaptureLog_Error;
    static int CaptureLog_Performace;
    static int CaptureLog_LostFrame;

    // 'N': No log output
    // 'I': Information; 'D': Debug; 'W':Warning; 'E':Error
    static void setLogLevel(const std::string& levels);

    static void output(const char* mainTag,
                        const char* subTag,
                           int type,
                           const char* pszFileName,
                           int iLineNum,
                           const char* pszFuncName,
                           const char* format,
                           ...);

    static void output_s(const char* mainTag, const char* subTag, int type, const char* format, ...);

private:
    static std::vector<int> m_logLevels;
};

#endif // CAPTURELOG_H
