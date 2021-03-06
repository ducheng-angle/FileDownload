#include <curl/curl.h>
#include <sys/sysinfo.h>
#include <sstream>
#include <string>
#include <iostream>

#include "Protocol.hpp"

/**********************************************************************
 * Function:       // get_download_file_length
 * Description:    // get file length in url
**********************************************************************/
int HttpProtocol::getDownloadFileLength(size_t &size) 
{
   int ret = 0;
   CURLcode code;
   double downloadFileLenth = 0;
   CURL *handle = curl_easy_init();
   curl_easy_setopt(handle, CURLOPT_URL, mURL.c_str());  
   curl_easy_setopt(handle, CURLOPT_HEADER, 1);    //只需要header头  
   curl_easy_setopt(handle, CURLOPT_NOBODY, 1);    //不需要body 
   curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
   curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 3L);
   curl_easy_setopt(handle, CURLOPT_AUTOREFERER, 1L);
   curl_easy_setopt(handle, CURLOPT_FORBID_REUSE, 1);
   curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/36.0.1985.143 Safari/537.36"); //user-agent
   code = curl_easy_perform(handle);
   if (code== CURLE_OK) {  
      code = curl_easy_getinfo(handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &downloadFileLenth);  
      if(code!=CURLE_OK)
      {
         ret = ERR_CURL_GET_FILE_SIZE_FAILED;
         std::cout << "get file size failed, error: " << ret << std::endl;
      }
      size = downloadFileLenth;
   } else {  
        ret = ERR_CURL_EASY_PERFORM_FAILED;
        std::cout << "curl perform failed when get file size, error: " << ret << std::endl;
   } 
   curl_easy_cleanup(handle);
   return ret;    
}
/**********************************************************************
 * Function:       // download file Slice
 * **********************************************************************/
int HttpProtocol::DownloadFile(FileInfo *info)
{
        //std::cout << "DownloadFile begin, fd:" << info->fd <<",offset: "<< info->offset << ",size: "<<info->size<<std::endl;
        int ret = 0;
        char range[64] = { 0 };
        snprintf (range, sizeof(range), "%ld-%ld", info->offset, info->offset + info->size - 1);

        CURL *handle = curl_easy_init();
        CURLcode code;
        curl_easy_setopt(handle, CURLOPT_URL, mURL.c_str());
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, Protocol::Write);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)info);
        curl_easy_setopt(handle, CURLOPT_RANGE, range);
        curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(handle, CURLOPT_TIMEOUT, 10);
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(handle, CURLOPT_FORBID_REUSE, 1L);
        curl_easy_setopt(handle, CURLOPT_LOW_SPEED_LIMIT, 1L);
        curl_easy_setopt(handle, CURLOPT_LOW_SPEED_TIME, 5L);
        curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 3L);
        curl_easy_setopt(handle, CURLOPT_AUTOREFERER, 1L);   
        curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/36.0.1985.143 Safari/537.36"); //user-agent    
        code = curl_easy_perform(handle);
        if(code !=CURLE_OK)
        {
           ret = ERR_CURL_EASY_PERFORM_FAILED;
           std::cout << "curl perform failed when down file, error: " << ret << std::endl;
        }
        curl_easy_cleanup(handle);
        
        return ret; 
}
