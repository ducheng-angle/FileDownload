#include <errno.h>
#include <curl/curl.h>
#include <fcntl.h>
#include <sys/sysinfo.h>
#include <assert.h>

#include "Log.hpp"
#include "Common.hpp"
#include "Protocol.hpp"
#include "Work.hpp"

int Work::Init(std::string protocol, std::string url)
{
    int ret = 0;
    size_t pos = 0;
    do {
        mProtocol = protocol;
        mURL = url;
        pos = mURL.find_last_of("/");
        if (pos != std::string::npos) {
            mName = mURL.substr(pos + 1);
        } else {
            mName = url;
        }

        /* curl global init */
        CURLcode code = curl_global_init(CURL_GLOBAL_ALL);
        if ( code != CURLE_OK ) {
            ret = CURL_GLOBAL_INIT;
            ERROR_LOG("curl global init failed, error: " << ret);
            break;
        }

    } while(0);

    DEBUG_LOG("init, error: " << ret
            << ", file name: " << mName);
    return ret;
}

Protocol *Work::NewProtocol()
{
    Protocol *pl = NULL;
    if (mProtocol == "http") {
        pl = new HttpProtocol(mURL);
    }


    return pl;
}

int Work::PrepareFile()
{
    int ret = 0;
    size_t size = 0;
    Protocol *pl = NULL;
    pl = NewProtocol();
    ret = pl->GetFileSize(size);
    if( ret != 0)
    {
       ERROR_LOG("get file size failed, error: " << ret);
    }
    else{
       mSize = size;
    }

    
    mFD = open(mName.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (mFD < 0) {
       ret = errno;
       ERROR_LOG("open file failed, error: " << ret);
    }
    
    DEBUG_LOG("init, error: " << ret
            << ", file size: " << mSize);
    return ret;

}

uint32_t Work::GetWorkCount()
{
     uint32_t count = 0;
     if (mSize < BLOCKSIZE)
     {
         count = 1;
     }
     else{
        if(mSize%BLOCKSIZE == 0)
        {
          count = mSize / BLOCKSIZE;
        }
        else{
          count = mSize / BLOCKSIZE +1;
        }
     }

     return count;
}

int Work::CreateWorks()
{
    int ret = 0;
    FileInfo info;
    info.fd = mFD;
    info.mErrno = 0;
    size_t size;
    Protocol *pl=NULL;
    uint32_t workCount = GetWorkCount();
    mProtocolQ.clear();
    for(int i =0;i<workCount;++i)
    {
        if(i<workCount -1)
        {
            size = mSize/workCount;
        }
        else{
            size = mSize -(workCount -1)*(mSize/workCount);
        }
        info.size = size;
        info.offset= i*(mSize/workCount);

        pl = NewProtocol();        
        assert(NULL != pl);
        ret = pl->Init();
        pl->SetFileInfo(info);
        if (ret != 0) {
            break;
        }
        mProtocolQ.push_back(pl);
    }
    
    return ret;
}

int Work::Start()
{
    int ret = 0;
    Protocol *pl = NULL;
    while (mProtocolQ.size() > 0) {
        pl = mProtocolQ.front();
        mProtocolQ.pop_front();
        do {
           ret = pthread_create(&mThreadId, NULL, Work::ThreadFunc,(void*)pl);
           if (ret != 0) {
               break;
           }
          else{
            WaitStop();
          }

        } while(0);
        delete pl;
        pl = NULL;
    
    }
    return ret;
}
#if 0
void Work::Run(Protocol *pl)
{
    int ret = 0;
    uint32_t retryCount = 0;   
    while(GetMaxRetryCount() > retryCount && ret!=0)
    {
         ret = pl->DownloadFile(pl->GetFileInfo());
         ++retryCount;
    }
    if (ret ! = 0) {
        mCC->SetErrCode(ret);
    }
}
#endif
void Work::WaitStop()
{
    if(mThreadId != -1)
    {
       pthread_join(mThreadId, NULL);
       mThreadId = -1;
    }
}

int   Work::StartWork()
{
    int ret = 0;

    do {
        ret = PrepareFile();
        if(ret != 0) {
            break;
        }
        ret = CreateWorks();
        if(ret != 0) {
            break;
        }
        ret = Start();
        if(ret != 0) {
            break;
        }
        ret = GetErrCode();
        if (ret != 0) {
            break;
        }
    } while(0);

    DEBUG_LOG("StartWork, error: " << ret);
    return ret;
}

void Work::Finit()
{
   for(int i =0;i<mProtocolQ.size();++i)
   {
      delete mProtocolQ[i];
   }
   mProtocolQ.clear();
   if(mFD > 0)
   {
      close(mFD);
      mFD = -1;
   }
   curl_global_cleanup();
}
