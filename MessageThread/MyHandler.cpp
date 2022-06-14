#include "MyHandler.h"
#include "MessageLooper.h"
#include "Log.h"

void MyHandler::onHandleMessage(Message* msg)
{
    // printf("onHandleMessage\n");
    switch(msg->type()) {
        case 0:
            printf("onHandleMessage exit\n");
            MessageLooper::looper()->quit();
            break;
        default:
            printf("onHandleMessage: type = %c\n", msg->type());
            int arg1 = msg->arg1();
            int arg2 = msg->arg2();
            break;
    }
}