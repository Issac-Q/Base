
#include "Thread.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
// #include "Utils.h"

//系统调用可以使用perror, strerror(errno)
//errno每个线程独有,是线程安全的
//pthread functions不设置errno的值,需要使用strerror(返回值)

Thread::Thread()
: mTid(0)
, mThreadName("\0")
{
    int ret = pthread_attr_init(&mAttr);
    if (ret != 0) {
        printf("Thread mAttr init failed %d [%s].", ret, strerror(ret));
    }

    ret = pthread_attr_setstacksize(&mAttr, 256*1024);
    if (0 != ret) {
        printf("Thread mAttr stacksize init failed %d [%s].", ret, strerror(ret));
    }
}

Thread::~Thread()
{
	stopThread();
}

bool Thread::startThread(const char *name)
{
    if (mTid != 0) {
        return true;
    }

    mQuit = false;

    int ret = pthread_create(&mTid, &mAttr, (void* (*)(void*))&threadProc, (void *)this);
    if (name != NULL) {
        pthread_setname_np(mTid, name);
    }

    if (ret != 0) {
        printf("Start thread fail %s!\n", strerror(ret));
        return false;
    }
    else {
        pthread_getname_np(mTid, mThreadName, THREAD_NAME_MAX_LENGTH);
        printf("start thread name [%s] successed\n", mThreadName);
        return true;
    }
}

const char* Thread::threadName()
{
    return mThreadName;
}

void Thread::threadProc(void* p)
{
	if(p == NULL) {
        printf("Thread::threadProc p is NULL\n");
        return;
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

//线程取消点非正常退出需要做清理工作
bool Thread::terminate()
{
    if (mTid == 0) {
        return true;
    }

    quit();

    if (mTid != pthread_self()) {
        int ret = pthread_cancel(mTid);
        if (0 == ret) {
            return true;
        }
        else {
            printf("Thread::terminate pthread_cancel failed: %s\n", strerror(ret));
            //唯一的错误就是线程找不到了
            mTid = 0;
            return false;
        }
    }
    else {
        exitSelf();
    }
}

//等线程运行完成退出
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
            printf("Thread::terminate failed error:%s", strerror(ret));
            return false;
        }
    }
    else {
        exitSelf();
    }
}

void Thread::exitSelf()
{
    pthread_detach(mTid);   //detach自动回收资源
    mTid = 0;
    pthread_exit(0);
}