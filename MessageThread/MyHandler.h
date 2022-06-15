#ifndef MY_HANDLER_H
#define MY_HANDLER_H

#include "MessageHandler.h"

class MyHandler : public MessageHandler
{
public:
    //java通过接口实例化的方式重写方法即可，可以直接使用接口的构造函数
    //这种匿名类的方式和正常继承实现的区别就在这
    DECLARE_CONSTRUCTOR(MyHandler)

protected:
    virtual void onHandleMessage(Message* msg);
};

#endif