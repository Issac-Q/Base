#ifndef MY_HANDLER_H
#define MY_HANDLER_H

#include "MessageHandler.h"

class MyHandler : public MessageHandler
{
public:
    DECLARE_CONSTRUCTOR(MyHandler)
    // MyHandler()
    // : MyHandler(NULL)
    // {

    // };

    // MyHandler(MessageLooper* looper)
    //  : MessageHandler(looper) 
    // {

    // };

protected:
    virtual void onHandleMessage(Message* msg);
};

#endif