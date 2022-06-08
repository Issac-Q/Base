#include <assert.h>
#include "FileWriter.h"

FileWriter::FileWriter()
: mOfs()
{

}

FileWriter::~FileWriter()
{
    close();
}

void FileWriter::open(const std::string& name, const std::ios_base::openmode& mode)
{
    if (!prepared()) {
        mOfs.open(name, mode);
        if (mOfs.is_open()) {
            setPrepared(true);
        }
        else {
            mOfs.close();
        }
    }
}

void FileWriter::write(const void* buffer, uint64_t size)
{
    if (prepared()) {
        mOfs.write((const char*)buffer, size);
    }    
}

void FileWriter::close()
{
    if (prepared()) {
        mOfs.close();
        // printf("close\n");
        setPrepared(false);
    }
}
