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
    //mErrno = 0;    
  }
  ~Work()
  {
    for(std::vector<FileInfo*>::iterator it=mFileBlock.begin();it!=mFileBlock.end();++it)
    {
        if(*it!=NULL){
           delete *it;
           *it=NULL;
        }
    }
    mFileBlock.clear();
  }

public:
  
  static void *ThreadFunc(void *arg)
  {
     FileInfo *info=(FileInfo*)arg;
     mErrno=pl->DownloadFile(info);
  }
  
public:
  
  int Init(std::string protocol,std::string url);
  int Prepare();
  int GetBlockCount();
  void PrepareFileBlock();
  int Start();
  void Wait();
  int DoWork();
  void SetWork(std::vector<FileInfo*>& Works)
  {
      mFileBlock.swap(Works);
  }

  void Finit();
  
private:
  static Protocol *pl;
  int mFD;
  std::string mURL;
  std::string mProtocol;
  std::vector<FileInfo*> mFileBlock; 
  std::string mName;
  pthread_t mThreadId;
  size_t mSize;
  std::vector<pthread_t> mIdVec;
  static int mErrno;
};

#endif

