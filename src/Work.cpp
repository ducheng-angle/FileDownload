#include <errno.h>
#include <curl/curl.h>
#include <iostream>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>
#include "Work.hpp"

Protocol *Work::pl=NULL;
int Work::Errno=0;

//初始化，http等协议
int Work::Init()
{
   int ret =0; 
   
   do{
   	if(Type == "http")
  	{
      	   pl = new HttpProtocol(URL);
           assert(pl!=NULL);
           pl->Init();
   	}
   	else{
      	   ret = ERR_PROTOCOL_INVALID;
           break;
   	}
        
   }while(0);
   std::cout << "init , error: "<< ret << std::endl;
   return ret;
}
//获取文件大小并打开
int Work::FileOperation()
{
   int ret = 0;
   size_t pos = 0;
   do{
     pos=URL.find_last_of("/");
     if(pos!=std::string::npos)
     {
        Name=URL.substr(pos+1);
     }
     else{
        ret = ERR_PARAMETER_INVALID;
        break;
     }

     FD=open(Name.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
     if(FD<0)
     {
        ret = errno;
        std::cout << "open file failed, error: " << ret << std::endl;
        break;
     }

     ret = pl->getDownloadFileLength(Size);
     if(ret!=0)
     {
       break;
     }
      
     //判断环境是否有足够的空间；
     if (lseek(FD, Size, SEEK_SET) == -1)
     {
        std::cout << "environment space not enough" << std::endl; 
     }
     lseek(FD, 0, SEEK_SET);
   }while(0);
   std::cout <<"file size: "<< Size << std::endl;
   return ret;
}
//获取文件分片数量
int Work::GetBlockCount()
{
    int count = 0;
    if(Size < BLOCKSIZE)
    {
      count = 1;
    }
    else{
       if(Size%BLOCKSIZE == 0)
       {
          count = Size/BLOCKSIZE;
       }
       else{
          count = Size/BLOCKSIZE +1;
       }

    }
    #if 1
    if(count > THREADNUM)
    {
       count = THREADNUM;
    }
    #endif
    std::cout << "count: " << count << std::endl; 
    return count;
}
//进行文件分片，设置分片的大小及偏移
void Work::PrepareFileBlock()
{
      int blockCount = GetBlockCount();
      
      for(int j =0 ;j<blockCount;++j)
      {
         FileInfo *info = (FileInfo *)malloc(sizeof(FileInfo));
         info->fd=FD;
         if(j<blockCount-1)
         {
            info->size  = Size/blockCount;
         }
         else{
            info->size = Size -(blockCount-1)*(Size/blockCount);
         }
         info->offset= j*(Size/blockCount);
         FileBlock.push_back(info);
      }
}
//创建多线程
int Work::Start()
{ 
   int ret = 0; 
   for(std::vector<FileInfo*>::iterator iter=FileBlock.begin();iter!=FileBlock.end();++iter)
   {
      
      do{
           ret = pthread_create(&ThreadId, NULL, Work::ThreadFunc,(void*)*iter);
           if (ret != 0) {
               break;
           }
           else{
              IdVec.push_back(ThreadId);
              
           }

      }while(0);
      if(ret!=0)
      {
        break;
      }
      
      std::cout << "thread,id: "<< ThreadId << ", file offset: "<< (*iter)->offset << " file size: " << (*iter)->size << std::endl;  
   }
   return ret;
}
//线程等待
void Work::Wait()
{
   #if 1
   for(std::vector<pthread_t>::iterator it = IdVec.begin();it!=IdVec.end();++it)
   {
      if(*it !=-1)
      {
        pthread_join(*it,NULL);
        *it =-1;
      }
   }
   #endif
   IdVec.clear();
}
//下载逻辑
int Work::DoWork()
{
  int ret = 0;
  do{
    ret = Init();
    if(ret !=0)
    {
         break;
    }

    ret =FileOperation();
    if(ret !=0)
    {
       break;
    }
    PrepareFileBlock();
    ret = Start();
    if(ret!=0)
    {
       break;
    }  
    Wait();
    if(Errno !=0)
    {
       ret = Errno;
       std::cout << "download failed, error: " << Errno << std::endl;
    }
    
  }while(0);

  Clear();

  if(ret==0)
  {
       std::cout << "download file success!!!"<<std::endl;
  }
  return ret;
} 
//清理
void Work::Clear()
{
   #if 1 
   for(std::vector<FileInfo*>::iterator it=FileBlock.begin();it!=FileBlock.end();++it)
   {
       if(*it!=NULL){
           delete *it;
           *it=NULL;
       }
   }
   FileBlock.clear();

   #endif
   if(pl != NULL)
   {  
      if(Type == "http")
      {
         pl->Clean();
         delete pl ;
         pl = NULL;
      }
   } 
   if(FD!=-1)
   {
      close(FD);
      FD=-1;
   }  

} 
