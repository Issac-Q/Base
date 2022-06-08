#ifndef BASIC_DEFINE_H
#define BASIC_DEFINE_H

#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <vector>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <memory>
#include <mutex>
#include <map>

#define _DEBUG_ 1

typedef struct _Element
{
    void*   _data;
    uint64_t _len;

    _Element()
    : _data(nullptr)
    , _len(0)
    {

    }

    _Element(const _Element& e)
    : _data(e._data)
    , _len(e._len)
    {

    }
}Element;

#endif