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
                    const size_t precision,
                    const size_t vectorLength,
                    const bool writable)
{
    if (! writable)
        os << "const ";

    os << PrecType::getPrimitiveName(precision);

    switch (vectorLength)
    {
        case (0) :
            os << PrecType::vecLength(precision);
            break;

        case (2) :
        case (4) :
            os << vectorLength;
            break;
    }
}


// declare kernel global array variable
string NameOf::globalvar(const size_t precision,
                         const size_t vectorLength,
                         const bool writable)
{
    stringstream ss;

    if (0 == vectorLength) // image
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

        memvar(ss, precision, vectorLength, writable);

        ss << "*";
    }

    return ss.str();
}

// declare kernel local array variable
string NameOf::localvar(const size_t precision,
                        const size_t vectorLength,
                        const bool writable)
{
    stringstream ss;

    ss << "__local ";

    memvar(ss, precision, vectorLength, writable);

    ss << "*";

    return ss.str();
}

// declare kernel private variable
string NameOf::privatevar(const size_t precision,
                          const size_t vectorLength,
                          const bool writable)
{
    stringstream ss;

    memvar(ss, precision, vectorLength, writable);

    return ss.str();
}

// OpenCL pragmas required for specific scalar types
string pragma_extension(const size_t precision,
                        const size_t deviceIndex)
{
    stringstream ss;

    switch (precision)
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
