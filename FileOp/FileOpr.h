#ifndef FILE_OPR_H
#define FILE_OPR_H

#include <string>
#include <fstream>
#include <map>

class FileOpr
{
public:
    FileOpr();
    virtual ~FileOpr();

    bool prepared();
    virtual void open(const std::string& name, const std::ios_base::openmode& mode) = 0;
    virtual void write(const void* buffer, uint64_t size) {};
    virtual void read(void* buffer, uint64_t size, uint64_t* readSize) {};
    virtual void close() = 0;

protected:
    void setPrepared(bool prepared);

private:
    bool mPrepared;
};

#endif