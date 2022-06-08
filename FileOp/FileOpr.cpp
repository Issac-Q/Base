#include <assert.h>
#include "FileOpr.h"

FileOpr::FileOpr()
: mPrepared(false)
{
}

FileOpr::~FileOpr()
{

}

bool FileOpr::prepared()
{
    return mPrepared;
}

void FileOpr::setPrepared(bool prepared)
{
    mPrepared = prepared;
}