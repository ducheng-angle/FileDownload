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
  Work()
  {
    mFD = -1;
    mThreadId = -1;
    mSize = 0;
    mErrno = 0;
    //pl = NULL;
  }
  ~Work()
  {
    #if 0
    if(pl !=NULL)
    {
       delete pl;
       pl=NULL;
    }
    #endif
  }

public:
  static void *ThreadFunc(void *arg)
  {
     FileInfo *info=(FileInfo*)arg;
     Work *wk = new Work();
     wk->Run(info);
     #if 1
     if(wk!=NULL)
     {
        delete wk;
        wk=NULL;
     }
     #endif
  }
public:
  int Init(std::string protocol,std::string url);
  int Prepare();
  int GetBlockCount();
  void PrepareFileBlock();
  int Start();
  void Wait();
  //Protocol *NewHttpProtocol();
  int DoWork();
  void Run(FileInfo* info);
  void Finit();
  
private:
  int mFD;
  std::string mURL;
  //Protocol *pl;
  std::vector<FileInfo*> mFileBlock;
  std::string mName;
  pthread_t mThreadId;
  size_t mSize;
  std::vector<pthread_t> mIdVec;
  int mErrno;
};

#endif

