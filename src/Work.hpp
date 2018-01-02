#ifndef __WORK_HPP__
#define __WORK_HPP__


#include <pthread.h>
#include <deque>
#include <stdint.h>
#include "Protocol.hpp"


class Work
{
public:
    Work(uint32_t retryCount = 3): mRetryCount(retryCount)
    {
        mErrno = 0;
        mThreadId = -1;
        mFD = -1;
        pthread_mutex_init(&mMutex, NULL);
        pthread_cond_init(&mCond, NULL);
    }
    ~Work()
    {
        pthread_mutex_destroy(&mMutex);
        pthread_cond_destroy(&mCond);
    }

public:
    int Init(std::string protocol, std::string url);
    void Finit();
    int Start();
    Protocol *NewProtocol();
    int PrepareFile();
    uint32_t GetWorkCount();
    int StartWork();
    int CreateWorks();
    void WaitStop();
    int32_t GetMaxRetryCount()
    {
        return mRetryCount;
    
    }
    
    void Run(Protocol *pl)
    {
      int ret = 0;
      //HttpProtocol *pl =(HttpProtocol*)arg;
      uint32_t retryCount = 0;   
      FileInfo info = pl->GetFileInfo();
      while(GetMaxRetryCount() > retryCount)
      {
         //FileInfo info = pl->GetFileInfo();
         ret = pl->DownloadFile(&info);
         ++retryCount;
         if(ret == 0)
         {
            break;
         }
      }
      if (ret != 0) {
        SetErrCode(ret);
      }
    }
    static void *ThreadFunc(void *arg)
    {
      HttpProtocol *pl =(HttpProtocol*)arg;
      Work *wk = new Work();
      wk->Run(pl);
    }

    inline int GetErrCode()
    {
        int err = 0;
        pthread_mutex_lock(&mMutex);
        err = mErrno;
        pthread_mutex_unlock(&mMutex);

        return err;
    }

    inline void SetErrCode(int err)
    {
        pthread_mutex_lock(&mMutex);
        mErrno = err;
        pthread_mutex_unlock(&mMutex);
    }


private:
    pthread_t mThreadId;
    std::deque<Protocol*> mProtocolQ;
    pthread_mutex_t mMutex;
    pthread_cond_t mCond;
    int mErrno;
    int mFD;
    std::string mURL;
    uint32_t mRetryCount;
    size_t mSize;
    std::string mProtocol;
    std::string mName;
};

#endif

