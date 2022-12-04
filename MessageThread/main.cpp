#include "MessageLooper.h"
#include "TestThread.h"

int main(void)
{
    // 1. attach a looper to the caller thread
    MessageLooper::attachMainLooper();
    //2. implement a handler(onHandleMessage) inherited from MessageHandler
    //3. new a handler instance whitin the thread that the message will be handle
    //4. new instance operation must be after the attach
    TestThread t, t2;
    t.startThread();
    // t2.startThread();
    //5. start to loop
    MessageLooper* looper = MessageLooper::looper();
    if (looper) {
        printf("main loop start\n");
        looper->loop();
    }

    //6. send the message from other threads(t) you needed

    //7. send a 0(type == 0) message from thread(t) to quit the loop
    t.stopThread();
    // t2.stopThread();
    pthread_exit(0);    //only "pthread_exit" way clean function can be called
}