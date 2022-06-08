#include "FileReader.h"

FileReader::FileReader()
: mIfs()
{

}

FileReader::~FileReader()
{
    close();
}

void FileReader::open(const std::string& name, const std::ios_base::openmode& mode)
{
    if (!prepared()) {
        mIfs.open(name, mode);
        if (mIfs.is_open()) {
            setPrepared(true);
        }
        else {
            mIfs.close();
        }
    }
}

void FileReader::read(void* buffer, uint64_t size, uint64_t* readSize)
{
    if (prepared()) {
        if (mIfs) {
            mIfs.read((char*)buffer, size);
            *readSize = mIfs.gcount();
        }
        else {
            printf("end of file\n");
        }
    }
}

void FileReader::close()
{
    if (prepared()) {
        mIfs.close();
        // printf("close\n");
        setPrepared(false);
    }
}
