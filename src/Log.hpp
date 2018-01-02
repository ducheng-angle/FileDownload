#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <iostream>

#define INFO_LOG(data)\
    {\
        std::cout <<  data << " [INFO] ["\
           << __FILE__ << ":"<< __FUNCTION__<< ":" << __LINE__ << "]\n";\
}\
  
#define DEBUG_LOG(data)\
    {\
        std::cout << data << " [DEBUG] ["\
           << __FILE__ << ":"<< __FUNCTION__<< ":" << __LINE__ << "]\n";\
}\


#define ERROR_LOG(data)\
    {\
        std::cout << data << " [ERROR] ["\
           << __FILE__ << ":"<< __FUNCTION__<< ":" << __LINE__ << "]\n";\
}\



#endif
