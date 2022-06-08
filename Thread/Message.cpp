#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Message.h"

#include <unistd.h>

Message::Message()
: mCustomData()
{

}
Message::Message(int type)
: mCustomData()
, mType(type)
{

}

Message::Message(int type, int arg1, int arg2)
: mCustomData()
, mType(type)
, mArg1(arg1)
, mArg2(arg2)
{

}

Message::Message(int type, int arg1, int arg2, const Element& customdata)
: mType(type)
, mArg1(arg1)
, mArg2(arg2)
, mCustomData(customdata)
{

}

void Message::handle()
{
    // printf("Message::handle => type:%d, arg1:%d, arg2:%d\n", mType, mArg1, mArg2);
    // usleep(10*1000);
}

Message::~Message()
{
    if (mCustomData._data) {
        free(mCustomData._data);
        mCustomData._data = NULL;
        //printf("release customData\n");
    }
}
