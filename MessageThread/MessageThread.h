#ifndef MESSAGE_THREAD_H
#define MESSAGE_THREAD_H

#include "Thread.h"
#include "MessageLooper.h"

class MeesageThread : public Thread
{
public:
    MeesageThread();
    ~MeesageThread();

    bool stopThread();

protected:
    virtual void run();

private:
    MessageLooper *mLooper;
};


#endif