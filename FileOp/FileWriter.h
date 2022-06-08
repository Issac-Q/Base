#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include "FileOpr.h"

class FileWriter : public FileOpr
{
public:
    FileWriter();
    virtual ~FileWriter();

    virtual void open(const std::string& name, const std::ios_base::openmode& mode);
    virtual void write(const void* buffer, uint64_t size);
    virtual void close();

private:
    std::ofstream mOfs;
};

#endif