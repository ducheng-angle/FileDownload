#ifndef __WORK_HPP__
#define __WORK_HPP__

#include <pthread.h>
#include <deque>
#include <vector>
#include <stdint.h>
#include "Protocol.hpp"
#include "Common.hpp"

class Protocol;
class Work
{
public:
  Work(std::string protocol,std::string url):Type(protocol),URL(url)
  {
    FD = -1;
    ThreadId = -1;
    Size = 0;   
  }
  ~Work()
  {
    #if 0
    for(std::vector<FileInfo*>::iterator it=FileBlock.begin();it!=FileBlock.end();++it)
    {
        if(*it!=NULL){
           delete *it;
           *it=NULL;
        }
    }
    FileBlock.clear();
    #endif
  }

public:
  
  static void *ThreadFunc(void *arg)
  {
     FileInfo *info=(FileInfo*)arg;
     if(Errno==0)
     {
         size_t size= info->size;
         if(size>BLOCKSIZE)
         {
            FileInfo file;
            file.fd=info->fd;
            off_t begin=info->offset;
            int count = 0;
            if(size%BLOCKSIZE==0)
            {
               count = size/BLOCKSIZE;
            }
            else
            {
               count = size/BLOCKSIZE+1;
            }
            for(int i =0 ;i< count ;++i)
            {
               file.offset=begin+i*(size/count);
               if(i<count-1)
               {
                  file.size=size/count;
               }
               else
               {
                  file.size=size-(size/count)*(count-1);
               }
               Errno=pl->DownloadFile(&file);
            }
            
         }
         else{
    	     Errno=pl->DownloadFile(info);
         }
     }
  }
  
public:
  
  int Init();
  int FileOperation();
  int GetBlockCount();
  void PrepareFileBlock();
  int Start();
  void Wait();
  int DoWork();
  void Clear();
  
private:
  static Protocol *pl;
  int FD;
  std::string URL;
  std::string Type;
  std::vector<FileInfo*> FileBlock; 
  std::string Name;
  pthread_t ThreadId;
  size_t Size;
  std::vector<pthread_t> IdVec;
  static int Errno;
};

#endif

