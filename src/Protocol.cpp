#include <curl/curl.h>
#include <fcntl.h>
#include <sys/sysinfo.h>
#include <sstream>
#include <string>

#include "Log.hpp"
#include "Common.hpp"
#include "Protocol.hpp"

int HttpProtocol::Init()
{
        mCURL = curl_easy_init();
        if (mCURL == NULL) {
            return 1;
        }

        curl_easy_setopt(mCURL, CURLOPT_URL, mURL.c_str());
        return 0;
}
static size_t GetHeader(char *ptr, size_t blockCount, size_t memBlockSize, void *arg)
{
    std::string *str = (std::string*)arg;
    (*str) += std::string(ptr);

    return blockCount * memBlockSize;
}

int HttpProtocol::GetFileSize(size_t &size)
{        
        int ret = 0;
        CURLcode code;
        CURL *handle= curl_easy_init();
        std::string header;
        double f_size = 0;
        curl_easy_setopt(handle, CURLOPT_URL, mURL.c_str());
        curl_easy_setopt(handle, CURLOPT_HEADER, 1);
        curl_easy_setopt(handle, CURLOPT_NOBODY, 1);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION,GetHeader);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &header);
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
        //ERROR_LOG("GetFileSize:" << mURL);
        code = curl_easy_perform(handle);
        if (code == CURLE_OK) {
            code = curl_easy_getinfo(handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &f_size);
            if (code != CURLE_OK) {
                size = 0;
                ret = CURL_GET_FILE_SIZE;
                DEBUG_LOG("SIZE: " << size);
            }
            size = f_size;
        } else {
            ret = CURL_GET_FILE_SIZE;
        }

        if (0 != ret) {
            ERROR_LOG("GetFileSize, failed, error: " << curl_easy_strerror(code)); 
        }
        DEBUG_LOG("SIZE: " << size);
        return ret;
 
}

int HttpProtocol::DownloadFile(FileInfo *fileInfo)
{
        int ret = 0;
        std::stringstream ss;
        ss << fileInfo->offset << "-" << (fileInfo->offset + fileInfo->size - 1) << std::endl;
        std::string Slice;
        ss >> Slice;
        //CURL *handle= curl_easy_init();
        CURLcode ccode;
        curl_easy_setopt(GetHandle(), CURLOPT_WRITEFUNCTION, Protocol::Write);
        curl_easy_setopt(GetHandle(), CURLOPT_WRITEDATA, (void*)fileInfo);
        curl_easy_setopt(GetHandle(), CURLOPT_RANGE, Slice.c_str());
        curl_easy_setopt(GetHandle(), CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(GetHandle(), CURLOPT_TIMEOUT, 10L);
        curl_easy_setopt(GetHandle(), CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(GetHandle(), CURLOPT_CUSTOMREQUEST, "GET");

        ccode = curl_easy_perform(GetHandle());
        if (CURLE_OK != ccode) {
            ret = CURL_DOWNLOAD_FAILED;
            ERROR_LOG("curl_easy_perform ,download file failed, errstr: " << curl_easy_strerror(ccode));
        } else {
            ret = (fileInfo->mErrno == 0 ? ret : fileInfo->mErrno);
        }
        return ret;
}

