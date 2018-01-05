#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <unistd.h>

enum ERROR
{
  PARAMETER_INVALID = 100,
  PROTOCOL_INVALID,
  CURL_EASY_PERFORM_FAILED,
  CURL_GET_FILE_SIZE_FAILED,
  FILE_WRITE_FAILED,
};

typedef struct file{
   int fd;
   off_t offset;
   size_t size;
}FileInfo;

#define BLOCKSIZE 16<<10
#define THREADNUM 128

#endif
