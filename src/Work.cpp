#include <errno.h>
#include <curl/curl.h>
#include <iostream>
#include <fcntl.h>
#include <assert.h>
#include "Work.hpp"
Protocol *pl;
int Work::Init(std::string protocol,std::string url)
{
   int ret =0; 
   size_t pos = 0;
   do{
   	mURL = url;
   	if(protocol == "http")
  	{
      	   pl = new HttpProtocol(mURL);
           pl->Init();
   	}
   	else{
      	   ret = PROTOCOL_INVALID;
           break;
   	}
        pos=mURL.find_last_of("/");
        if(pos!=std::string::npos)
        {
          mName=mURL.substr(pos+1);
        }
        else{
          mName = mURL;
        }
   }while(0);
   std::cout << "init , error: "<< ret << std::endl;
   return ret;
}

int Work::Prepare()
{
   int ret = 0;
   size_t size = 0;
   do{
     ret = pl->getDownloadFileLength(size);
     if(ret!=0)
     {
       break;
     }
     mSize = size;
     mFD=open(mName.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
     if(mFD<0)
     {
        ret = errno;
        std::cout << "open file failed, error: " << ret << std::endl;
        break;
     }
   }while(0);
   std::cout <<"file size: "<< mSize << std::endl;
   return ret;
}

int Work::GetBlockCount()
{
    int count = 0;
    if(mSize < BLOCKSIZE)
    {
      count = 1;
    }
    else{
       if(mSize%BLOCKSIZE == 0)
       {
          count = mSize/BLOCKSIZE;
       }
       else{
          count = mSize/BLOCKSIZE +1;
       }

    } 
    return count;
}

void Work::PrepareFileBlock()
{
   int blockCount = GetBlockCount();
   //FileInfo *info = NULL;
   //info->fd=mFD;
   //mFileBlock.clear();
   if(blockCount > THREADNUM)
   {
      for(int i=0;i<THREADNUM;++i)
      {
         FileInfo *info;
         info->fd=mFD;

         if(i<THREADNUM-1)
         {
            info->size = mSize/THREADNUM;
         }
         else{
            info->size = mSize - (THREADNUM-1)*(mSize/THREADNUM);
         }
         info->offset=i*(mSize/THREADNUM);
     
         mFileBlock.push_back(info);
      }
   }
   else{
      for(int j =0 ;j<blockCount;++j)
      {
         FileInfo *info;
         info->fd=mFD;
         if(j<blockCount-1)
         {
            info->size  = mSize/blockCount;
         }
         else{
            info->size = mSize -(blockCount-1)*(mSize/blockCount);
         }
         info->offset= j*(mSize/blockCount);
         mFileBlock.push_back(info);
      }
   }
}

int Work::Start()
{ 
   int ret = 0;
   for(std::vector<FileInfo*>::iterator iter=mFileBlock.begin();iter!=mFileBlock.end();++iter)
   {
      
      do{
           ret = pthread_create(&mThreadId, NULL, Work::ThreadFunc,(void*)*iter);
           if (ret != 0) {
               break;
           }
           else{
              mIdVec.push_back(mThreadId);
           }

      }while(0);
   }
   return ret;
}
#if 0
Protocol *Work::NewHttpProtocol()
{
      Protocol * pl =new HttpProtocol(mURL);
      return pl;
}
#endif
void Work::Run(FileInfo* info)
{
    mErrno = pl->DownloadFile(info);
}

void Work::Wait()
{
   #if 1
   for(std::vector<pthread_t>::iterator it = mIdVec.begin();it!=mIdVec.end();++it)
   {
      if(*it !=-1)
      {
        pthread_join(*it,NULL);
        *it =-1;
      }
   }
   #endif
   mIdVec.clear();

}

int Work::DoWork()
{
  int ret = 0;
  do{
    PrepareFileBlock();
    ret = Start();
    if(ret!=0)
    {
       break;
    }  
    Wait();
    if(mErrno !=0)
    {
       ret = mErrno;
       std::cout << "download failed, error: " << mErrno << std::endl;
    }
  }while(0);
  
  if(ret==0){
    std::cout << "download file success!!!" << std::endl; 
  }
  
  return ret;
} 

void Work::Finit()
{
  
   if(!mFileBlock.empty())
   {
       std::vector<FileInfo*>().swap(mFileBlock);
   }
   mFileBlock.clear();
   //pl->Finit();
   if(pl != NULL)
   {  
      pl->Finit();
      delete pl ;
      pl = NULL;
   } 
   if(mFD>0)
   {
      close(mFD);
   }  

} 
