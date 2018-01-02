#ifndef __PROTOCOL_HPP__
#define __PROTOCOL_HPP__
#include <unistd.h>
#include <curl/curl.h>
#include <errno.h>
#include <string>

#include "Common.hpp"
#include "Log.hpp"

class Protocol
{
public:
    Protocol(std::string url):mURL(url)
    {
    }
    ~Protocol()
    {
    }
public:
    static size_t Write(char *ptr, size_t blockCount, size_t memBlockSize, void *arg)
    {
        FileInfo *info = (FileInfo*)arg;

        size_t total = blockCount * memBlockSize;
        size_t writed = 0;
        ssize_t ret = 0;
        off_t offset = info->offset;

        while (total >0) {
            ret = pwrite(info->fd, ptr + writed, total, offset);
            if (ret < 0) {
                info->mErrno = errno;
                break;
            }

            total -= ret;
            writed += ret;
            offset += ret;
        }

        info->offset = offset;

        return blockCount * memBlockSize;
    }

    virtual int DownloadFile(FileInfo* fileInfo) = 0;
    virtual int GetFileSize(size_t &size) = 0;
    virtual void SetFileInfo(FileInfo info) = 0;
    virtual int Init() = 0;
    virtual FileInfo GetFileInfo() = 0;
public:
   std::string mURL;
};

class HttpProtocol: public Protocol
{  
public:
   HttpProtocol(std::string url):Protocol(url)
  {
     
  }
  ~HttpProtocol()
  {

  }
public:
  int Init();
  CURL *GetHandle()
  {
        return mCURL;
  }
  
  void SetFileInfo(FileInfo info)
  {
        mFileInfo = info;
  }
  FileInfo GetFileInfo()
  {
      return mFileInfo;
  }
  //size_t GetHeader(char *ptr, size_t blockCount, size_t memBlockSize, void *arg);
  int GetFileSize(size_t &size);
  virtual int DownloadFile(FileInfo *fileInfo);
private:
  CURL *mCURL;
  //std::string mURL;
  size_t mSize;
  FileInfo mFileInfo; 
  
};
#endif
