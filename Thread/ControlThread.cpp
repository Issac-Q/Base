#include "ControlThread.h"
#include <unistd.h>

ControlThread::ControlThread()
: mRunning(false)
, mRunSemExitStart(false)
, mRunSemExitFinished(false)
{
    sem_init(&mRunSem, 0, 0/*init value*/);
}

ControlThread::~ControlThread()
{
    stopThread();
    sem_destroy(&mRunSem);
}

bool ControlThread::stopThread()
{
    startRun(); //let the thread exit from the block
    Thread::stopThread();
}

void ControlThread::startRun()
{
    if (!mRunning) {
        mRunning = true;
    }

    wakeUpIfNeed();
}

void ControlThread::stopRun()
{
    if (mRunning) {
        mRunning = false;
    }
}

void ControlThread::run()
{
    if (!mRunning) {
        //block when not running
        int semVal;
        onStopRun();
        do {
            sem_getvalue(&mRunSem, &semVal);
            sem_wait(&mRunSem);
        }while(semVal > 0);
        sem_getvalue(&mRunSem, &semVal); 
        //wake up when running, loop to check the "mRunning" modify by other thread
        while (!mRunning) {
            sleep(1);
        }
        onStartRun();
    }
    // if (mRunSemExitStart) {
    //     //make sure the destory run after the sem_wait
    //     mRunSemExitFinished = true;
    // }
    // else {
        onRun();
    // }    
}

void ControlThread::wakeUpIfNeed()
{
    int semVal;
    sem_getvalue(&mRunSem, &semVal);
    if (semVal < 1) {
        sem_post(&mRunSem);
        sem_getvalue(&mRunSem, &semVal);
    }
}