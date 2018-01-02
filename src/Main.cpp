#include <getopt.h>
#include <errno.h>
#include <assert.h>
#include <iostream>
#include <string>

#include "Log.hpp"
#include "Work.hpp"

void Usage()
{
    std::string usage = "\nexample: Download -p http -u https://www.baidu.com\n"\
                        "\t-p currently support http\n"\
                        "\t-u url of the download file\n";

    std::cout << usage << std::endl;
}

int ParseArgs(int argc, char *argv[], std::string& protocol, std::string& url)
{
    int err = 0;
    int opt;
    while ((opt = getopt(argc, argv, "p:u:")) != -1) {
        switch(opt) {
            case 'p':
                protocol = std::string(optarg);
                break;
            case 'u':
                url = std::string(optarg);
                break;
            default:
                Usage();
                ERROR_LOG("invalid argreement: " << opt); 
                err = EINVAL;
                break;
       }
    }

    if (protocol.empty()|| protocol != "http") {
        ERROR_LOG("-p <PROTOCOL> is requested,and currently only support http");
        err = EINVAL;
    }
    if (url.empty()) {
        ERROR_LOG("-u <URL> is requested");
        err = EINVAL;
    }

    return err;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    std::string protocol;
    std::string url; 
    Work *wk = NULL;
    ret = ParseArgs(argc,argv,protocol,url);
    if( ret != 0)
    {
        goto OUT;
    }
    //Work *wk = NULL;
    wk = new Work();
    assert( wk != NULL);
    ret = wk->Init(protocol,url);
    if( ret != 0)
    {
        goto OUT;
    }
    
    ret= wk->StartWork();
    if( ret != 0)
    {
        goto OUT;
    }
    
OUT:
   DEBUG_LOG("download file , error: " << ret);
   if(wk != NULL){
     wk->Finit();
     delete wk;
     wk = NULL;
   }
   return ret;
   
}
