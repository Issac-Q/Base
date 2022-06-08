#ifndef CONTROL_THREAD_H
#define CONTROL_THREAD_H

#include "BasicDefine.h"
#include "Thread.h"

class ControlThread : public Thread
{
public:
    ControlThread();
    virtual ~ControlThread();

    virtual bool stopThread() final;

protected:
    void startRun();
    void stopRun();

    virtual void onStartRun() {};
    virtual void onStopRun() {};

    virtual void run() final;
    virtual void onRun() = 0;

private:
    void wakeUpIfNeed();

    bool mRunning;
    bool mRunSemExitStart;
    bool mRunSemExitFinished;
    sem_t mRunSem;
};
#endif