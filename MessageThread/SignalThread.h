#ifndef TEST_THREAD_H
#define TEST_THREAD_h

#include "Thread.h"

class SignalThread : public Thread
{
public:
    SignalThread(Thread* thread);

protected:
    virtual void run();

private:
    Thread* mThread;
};

#endif