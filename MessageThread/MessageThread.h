#ifndef MESSAGE_THREAD_H
#define MESSAGE_THREAD_H

#include "Thread.h"
#include "MessageLooper.h"

class MeesageThread : public Thread
{
public:
    MeesageThread();
    ~MeesageThread();

protected:
    virtual void run();
    virtual void postRun();
    virtual void onStop();

private:
    MessageLooper *mLooper;
};


#endif