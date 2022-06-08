#ifndef FILE_READER_H
#define FILE_READER_H

#include "FileOpr.h"

class FileReader : public FileOpr
{
public:
    FileReader();
    virtual ~FileReader();

    virtual void open(const std::string& name, const std::ios_base::openmode& mode);
    virtual void read(void* buffer, uint64_t size, uint64_t* readSize);
    virtual void close();

private:
    std::ifstream mIfs;
};

#endif