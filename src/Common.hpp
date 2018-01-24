#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <unistd.h>

enum ERRNO
{
  ERR_PARAMETER_INVALID = 100,
  ERR_PROTOCOL_INVALID,
  ERR_CURL_EASY_PERFORM_FAILED,
  ERR_CURL_GET_FILE_SIZE_FAILED,
  ERR_CURL_DOWNLOAD_FAILED,
  ERR_FILE_WRITE_FAILED,
};

typedef struct file{
   int fd;
   off_t offset;
   size_t size;
}FileInfo;

#define BLOCKSIZE size_t(1*1024*1024)
#define THREADNUM 32

#endif
