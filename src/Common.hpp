#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <unistd.h>

enum ERROR {
    CURL_GLOBAL_INIT = -5 ,
    CURL_EASY_INIT, 
    CURL_EASY_PERFORM,
    CURL_GET_FILE_SIZE, 
    CURL_DOWNLOAD_FAILED, 
};
typedef struct File{
    int fd;
    off_t offset;
    size_t size;
    int mErrno;
}FileInfo;

#define BLOCKSIZE 16<<10

#endif
