// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <fstream>

#include "chai/chai.h"
#include "Scheduler.hpp"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// virtual machine management

void init(istream& configSpecStream)
{
    Scheduler::singleton().init(configSpecStream);
}

void init(const string& configSpecFileName)
{
    ifstream configSpecStream(configSpecFileName.c_str());
    init(configSpecStream);
}

void shutdown(void)
{
    Scheduler::singleton().shutdown();
}

}; // namespace chai
