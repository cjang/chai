// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <iostream>

#include "Logger.hpp"

using namespace std;

namespace chai_internal {

Logger::Logger(void)
{
    pthread_mutex_init(&_printMtx, NULL);
}

Logger::~Logger(void)
{
    pthread_mutex_destroy(&_printMtx);
}

Logger& Logger::singleton(void)
{
    static Logger obj;
    return obj;
}

void Logger::print(const string& msgText,
                   const char* funcName)
{
    pthread_mutex_lock(&_printMtx);

    const pthread_t ptself = pthread_self();

    if (! _printCount.count(ptself))
        _printCount[ptself] = 0;

    const size_t msgNum = _printCount[ptself]++;

    pthread_mutex_unlock(&_printMtx);

    cerr << "Logger[ " << msgNum << " ][ " << funcName << " ] "
         << msgText
         << endl;
}

}; // namespace chai_internal
