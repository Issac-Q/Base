
#include "Thread.h"
#include "stdio.h"
#include <errno.h>
#include <string.h>
// #include "Utils.h"

#define _GNU_SOURCE

Thread::Thread()
: mQuit(false)
, mThreadName("\0")
{
    mTid = 0;
    int ret = pthread_attr_init(&mAttr);
    if (0 != ret) {
        (void)printf("Thread mAttr init failed %d [%s].", ret, strerror(ret));
    }

    ret = pthread_attr_setstacksize(&mAttr, 256*1024);
    if (0 != ret) {
        (void)printf("Thread mAttr stacksize init failed %d [%s].", ret, strerror(ret));
    }
}

Thread::~Thread()
{
	stopThread();
}

bool Thread::terminate()
{
    if (mTid == 0) {
        return true;
    }

    mQuit = true;

    if (mTid != pthread_self()) {
        int ret = pthread_cancel(mTid);
        if (0 == ret) {
            return true;
        }
        else {
            //printf("<<<< Stop thread [%s] (%lu, 0x%lx) join failed %d [%s].", m_szThreadName, m_dwThreadID, m_dwThreadID, ret, strerror(ret));
            return false;
        }
    }
    else {
        return true;
    }
}


bool Thread::startThread(const char *name)
{
    if (mTid != 0) {
        return false;
    }

    mQuit = false;

    // printf("start thread name:%s\n", name);
    int ret = pthread_create(&mTid, &mAttr, (void* (*)(void*))&threadProc, (void *)this);
    if (name != NULL) {
        pthread_setname_np(mTid, name);
    }

    if (ret != 0) {
        (void)printf("Start TEMP1 thread fail!\n");
        return false;
    }

    // Utils::copyString(mThreadName, name, THREAD_NAME_MAX_LENGTH);
    return true;
}


void Thread::threadProc(void* p)
{
	if(p == NULL) {
        return ;
    }
	((Thread*)p)->execRun();
}

void Thread::execRun()
{
	preRun();
	loop();
	postRun();
}

void Thread::quit()
{
    mQuit = true;
}

void Thread::loop()
{
    while(!mQuit) {
        run();
    }
}

bool Thread::stopThread()
{
	if (mTid == 0) {
        return true;
    }

    quit();

    if (mTid != pthread_self()) {
        int ret = pthread_join(mTid, NULL);
        if (0 == ret) {
            mTid = 0;
            return true;
        }
        else {
            (void)printf("<<<< Stop TEMP1 thread  join failed \n.");
            return false;
        }
    }
    else {
        mTid = 0;
        return true;
    }
}

const char* Thread::threadName()
{
    return mThreadName;
}