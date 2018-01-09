#include <curl/curl.h>
#include <sys/sysinfo.h>
#include <sstream>
#include <string>
#include <iostream>

#include "Protocol.hpp"

int HttpProtocol::getDownloadFileLength(size_t &size) 
{
   int ret = 0;
   CURLcode code;
   double downloadFileLenth = 0;
   CURL *handle = curl_easy_init();
   curl_easy_setopt(handle, CURLOPT_URL, mURL.c_str());  
   curl_easy_setopt(handle, CURLOPT_HEADER, 1);    //只需要header头  
   curl_easy_setopt(handle, CURLOPT_NOBODY, 1);    //不需要body 
    
   code = curl_easy_perform(handle);
   if (code== CURLE_OK) {  
      code = curl_easy_getinfo(handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &downloadFileLenth);  
      if(code!=CURLE_OK)
      {
         ret =CURL_GET_FILE_SIZE_FAILED;
         std::cout << "get file size failed, error: " << ret << std::endl;
      }
      size = downloadFileLenth;
   } else {  
        ret = CURL_EASY_PERFORM_FAILED;
        std::cout << "curl perform failed when get file size, error: " << ret << std::endl;
   }
   //std::cout << "size: " << mURL << std::endl;  
   curl_easy_cleanup(handle);
   return ret;    
}
//std::stringstream ss;
//std::string blockRange;
int HttpProtocol::DownloadFile(FileInfo *info)
{
        std::cout << "DownloadFile begin, fd:" << info->fd <<",offset: "<< info->offset << ",size: "<<info->size<<std::endl;
        int ret = 0;
        std::stringstream ss;
        ss << info->offset << "-" << (info->offset + info->size - 1) << std::endl;
        std::string blockRange;
        ss >> blockRange;
        CURL *handle = curl_easy_init();
        CURLcode code;
        curl_easy_setopt(handle, CURLOPT_URL, mURL.c_str());
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, Protocol::Write);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)info);
        curl_easy_setopt(handle, CURLOPT_RANGE, blockRange.c_str());
        curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(handle, CURLOPT_TIMEOUT, 10);
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "GET");
        code = curl_easy_perform(handle);
        if(code !=CURLE_OK)
        {
           ret = CURL_EASY_PERFORM_FAILED;
           std::cout << "curl perform failed when down file, error: " << ret << std::endl;
        }
        curl_easy_cleanup(handle);
        //std::cout << "DownloadFile,error: " << ret << std::endl;
        return ret; 
}
