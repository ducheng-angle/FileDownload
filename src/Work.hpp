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
     Errno=pl->DownloadFile(info);
     
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

