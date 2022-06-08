#ifndef MESSAGE_H
#define MESSAGE_H

#include "BasicDefine.h"

//为了避免在线程函数的循环里面switch case的处理消息
//将这个功能下放到一个listener中
//但是如果looper要处理很多不同人负责的消息,出现两个问题, 一个是listener的注册问题, 二是lsitener中的switch case就需要频繁的改动
//所以listener的方式适用于只有一个人使用的简单case,
//多人多模块使用的情况,需要将listener分配给message,取名为handler,一个消息或者一类消息使用自己的handler来处理
//这样大家自己玩自己的,互不干扰
//定义一个专门处理消息的接口--handler
//消息的发送者同时提供一个消息的处理者

//作为异步任务,需要关心就是:1.发送什么样的数据给队列,2.在那个线程怎么处理这些消息
//将消息发给线程的消息队列,消息带有一个处理者,异步回调就调用到处理者handler的函数
//为了更加解耦和使用简洁, 更进一步:将消息剥离出来,只与它最关心的处理者打交道
//把异步调用的最核心两步发送和调用都放在handler里面实现
//1. 继承handler实现自己的MyHandler, 根据messageId 实现处理消息函数
//2. msg = handler.obtainMessage()
//3. myhandler.sendMessage()

//4. 将handler绑定到消息队列处理线程,但不直接很线程打交道,而是得到线程的消息队列,并和消息队列打交道
//5. 进一步线程和消息循环体(Looper)也应该分离,线程的函数体负责运行Looper的loop
//6. 将handler绑定到消息循环体Looper,但不直接跟循环体打交道,而是得到Looper的消息队列,并和消息队列打交道
//7. message->handler->messageQueue
//8. thread->run->Looper.loop->msg.handler.handlerMessage

class Message
{
public:
    Message();
    Message(int type);
    Message(int type, int arg1, int arg2);
    Message(int type, int arg1, int arg2, const Element& customdata);
    virtual ~Message();

    int type() { return mType; };
    int arg1() { return mArg1; };
    int arg2() { return mArg2; };
    Element& customData() { return mCustomData; };
    static Message& obtianMessage() {};   //TODO reuse the message object
    virtual void handle();

    Message* mNext;      //implement signal linked list
    uint64_t mWhen;      //time to handle

private:
    int mType;
    int mArg1;
    int mArg2;
    Element mCustomData;
};

#endif