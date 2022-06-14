#include "TestThread.h"
#include "Log.h"
#include "MessageLooper.h"

TestThread::TestThread()
: myHandler(NULL)
{

}

TestThread::~TestThread()
{
    if (myHandler) {
        delete myHandler;
        myHandler = NULL;
    }
}

bool TestThread::startThread(const char *name)
{
    // myHandler = new MyHandler();
    Thread::startThread(name);
}

void TestThread::preRun()
{
    myHandler = new MyHandler(MessageLooper::mainLooper());
}

void TestThread::run()
{
    while (1) {
        char ch = getchar();
        if (ch == 'a') {
            printf("sendMessage a\n");
            myHandler->sendMessage((int)ch);
        }
        if (ch == 'b') {
            printf("sendMessage b\n");
            myHandler->sendMessage((int)ch);
        }
        if (ch == 'c') {
            printf("sendMessage c\n");
            myHandler->sendMessage((int)ch);
        }
        if (ch == 'e') {
            printf("sendMessage e\n");
            myHandler->sendMessage(0);            
            break;
        }
    }
    quit();
}
