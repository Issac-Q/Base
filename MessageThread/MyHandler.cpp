#include "MyHandler.h"
#include "MessageLooper.h"

void MyHandler::onHandleMessage(Message* msg)
{
    switch(msg->type()) {
        case 0:
            MessageLooper::looper()->quit();
            break;
        case 1:
            int arg1 = msg->arg1();
            int arg2 = msg->arg2();
            break;
    }
}