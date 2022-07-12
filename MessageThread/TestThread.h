#ifndef TEST_THREAD_H
#define TEST_THREAD_h

#include "Thread.h"
#include "MyHandler.h"

class TestThread : public Thread
{
public:
    TestThread();
    virtual ~TestThread();

    bool startThread(const char *name = NULL);

protected:
    virtual void preRun();
    virtual void run();
    virtual void onStop();
    virtual void onStopped(pthread_t tid);

    MessageHandler *myHandler;
};

#endif