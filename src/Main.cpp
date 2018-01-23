#include <getopt.h>
#include <iostream>
#include <errno.h>
#include <assert.h>
#include "Common.hpp"
#include "Work.hpp"

int ParseArgs(int argc,char *argv[],std::string &protocol,std::string &url)
{
   int ret =0;
   int opt ;
   const std::string patterns[] = {"http://", "https://"};
   while((opt=getopt(argc,argv,"p:u:"))!= -1)
   {
       switch(opt)
       {
          case 'p':
              protocol=std::string(optarg);
              break;
          case 'u':
              url=std::string(optarg);
              break;
          default:
              ret = PARAMETER_INVALID;
              std::cout << "invalid argreement: " << opt << std::endl;
              break;
       }
       if(ret != 0)
       {
          break;
       }
   }
   
   if(protocol.empty() || protocol!="http")
   {
       ret = PARAMETER_INVALID;
       std::cout << "-p <PROTOCOL> is requested,and currently only support http" << std::endl;
   } 
  
   if(url.empty())
   {
      ret =PARAMETER_INVALID;
      std::cout << "-u <URL> is requested" << std::endl;
   }
  
   if(url.substr(0, patterns[0].size()) != patterns[0] && url.substr(0, patterns[1].size()) != patterns[1])
   { 
        ret =PARAMETER_INVALID;
        std::cout << "-u <URL> invalid" << std::endl;
   }
   
   return ret;   
}

int main(int argc , char *argv[])
{
    int ret = 0;
    std::string protocol ;
    std::string url;
    Work *wk = new Work();
    assert( wk!=NULL);
    do{
      ret = ParseArgs(argc,argv,protocol,url);
      if(ret !=0)
      {
         break;
      }
      
      ret = wk->Init(protocol,url);
      if(ret !=0)
      {
         break;
      }
    
      ret = wk->Prepare();
      if(ret !=0)
      {
         break;
      }
      ret=wk->DoWork();
      if(ret !=0)
      {
         break;
      }
    }while(0);
   
    if(wk!=NULL);
    {
        wk->Finit();
        delete wk;
        wk=NULL;
    }
    //std::cout << "download file, error: " << ret << std::endl; 
    return ret;
}
