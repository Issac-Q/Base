#include "MessageLooper.h"
#include "MyHandler.h"
int main(void)
{
    //1. attach a looper to the caller thread
    MessageLooper.attach();
    //2. implement a handler(onHandleMessage) inherited from MessageHandler
    //3. new a handler instance whitin the thread that the message will be handle
    //4. new instance operation must be after the attach
    MyHandler myHandler = new MyHandler();
    //5. start to loop
    MessageLooper* looper = MessageLooper::looper();
    if (looper) {
        looper->loop();
    }

    //6. send the message from other threads you needed
    myHandler->sendMessage(1);

    //7. send a 0(type == 0) message to quit the loop
}