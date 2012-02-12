// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_LOGGER_HPP_
#define _CHAI_LOGGER_HPP_

#include <map>
#include <pthread.h>
#include <string>

namespace chai_internal {

////////////////////////////////////////
// Logging
// (may be useful for introspection)

class Logger
{
    pthread_mutex_t               _printMtx;
    std::map< pthread_t, size_t > _printCount;

    Logger(void);

public:
    ~Logger(void);

    static Logger& singleton(void);

    void print(const std::string& msgText,
               const char* funcName);
};

#ifdef __LOGGING_ENABLED__
#define LOGGER(msgText) Logger::singleton().print(msgText, __func__);
#else
#define LOGGER(msgText) ;
#endif

}; // namespace chai_internal

#endif
