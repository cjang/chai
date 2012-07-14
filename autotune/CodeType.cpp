// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>

#include "CodeType.hpp"
#include "OCLhacks.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// type related stuff

void NameOf::memvar(ostream& os,
                    const size_t PREC,
                    const size_t vecLen,
                    const bool writable)
{
    if (! writable)
        os << "const ";

    os << PrecType::getName(PREC);

    switch (vecLen)
    {
        case (0) :
            os << PrecType::vecLength(PREC);
            break;

        case (2) :
        case (4) :
            os << vecLen;
            break;
    }
}


// declare kernel global array variable
string NameOf::globalvar(const size_t PREC,
                         const size_t vecLen,
                         const bool writable)
{
    stringstream ss;

    if (0 == vecLen) // image
    {
        if (writable)
            ss << "__write_only ";
        else
            ss << "__read_only ";

        ss << "image2d_t";
    }
    else // memory buffer
    {
        ss << "__global ";

        memvar(ss, PREC, vecLen, writable);

        ss << "*";
    }

    return ss.str();
}

// declare kernel local array variable
string NameOf::localvar(const size_t PREC,
                        const size_t vecLen,
                        const bool writable)
{
    stringstream ss;

    ss << "__local ";

    memvar(ss, PREC, vecLen, writable);

    ss << "*";

    return ss.str();
}

// declare kernel private variable
string NameOf::privatevar(const size_t PREC,
                          const size_t vecLen,
                          const bool writable)
{
    stringstream ss;

    memvar(ss, PREC, vecLen, writable);

    return ss.str();
}

// OpenCL pragmas required for specific scalar types
string pragma_extension(const size_t PREC,
                        const size_t deviceIndex)
{
    stringstream ss;

    switch (PREC)
    {
        case (PrecType::UInt32) :
        case (PrecType::Int32) :
        case (PrecType::Float) :
            // nothing required
            break;

        case (PrecType::Double) :
            ss << OCLhacks::singleton().pragmaFP64(deviceIndex);
            break;
    }

    ss << endl;

    return ss.str();
}

}; // namespace chai_internal
