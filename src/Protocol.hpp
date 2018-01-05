#ifndef __PROTOCOL_HPP__
#define __PROTOCOL_HPP__
#include <unistd.h>
#include <curl/curl.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <string.h>
#include "Common.hpp"

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
  static size_t Write(char *ptr,  size_t blockCount, size_t blockSize, void *arg)
  {
     FileInfo *info = (FileInfo*)arg;
     size_t total = blockCount*blockSize;
     size_t writed= 0;
     size_t ret = 0;
     off_t offset = info->offset;
     while(total>0)
     {
        ret = pwrite(info->fd, ptr+writed,total,offset);
        if(ret <0)
        {
          writed = 0;   
          std::cout << "pwrite failed: " << strerror(errno) << std::endl;
          break;
        }
        total -=ret;
        offset +=ret;
        writed +=ret;
     }
     info->offset=offset;
     return writed;
  }
  virtual void Init() = 0;
  virtual void Finit() = 0;
  virtual int getDownloadFileLength(size_t &size) = 0;
  virtual int DownloadFile(FileInfo *info) = 0;
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
   virtual void Init()
   {
     curl_global_init(CURL_GLOBAL_DEFAULT);
   }
   virtual void Finit()
   {
     curl_global_cleanup();
   }
   virtual int getDownloadFileLength(size_t &size);
   virtual int DownloadFile(FileInfo *info);
};
#endif



