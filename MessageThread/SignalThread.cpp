#include "SignalThread.h"
#include "Log.h"

SignalThread::SignalThread(Thread* thread)
: mThread(thread)
{

}

void SignalThread::run()
{
    while (1) {
        printf("sgt getchar input\n");
        char ch = getchar();
        // if (ch == 'a') {
        //     printf("sendMessage a\n");
        //     myHandler->sendMessage((int)ch);
        // }
        // if (ch == 'b') {
        //     printf("sendMessage b\n");
        //     myHandler->sendMessage((int)ch);
        // }
        // if (ch == 'c') {
        //     printf("sendMessage c\n");
        //     myHandler->sendMessage((int)ch);
        // }
        printf("sgt receiver input\n");
        if (ch == 'e') {
            printf("stop handler thread\n");
            mThread->stopThread();
            break;
        }
    }
    quit();
}
