// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>

#include "CodeImageSample.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// images and texture sampling

// SamplerVar
string SamplerVar::name(void) const
{
    return "sampler";
}

string SamplerVar::declaredName(void) const
{
    return "const sampler_t sampler";
}

// ImageSampler
string ImageSampler::name(void) const
{
    return "CLK_FILTER_NEAREST | "
           "CLK_NORMALIZED_COORDS_FALSE | "
           "CLK_ADDRESS_NONE";
}

// ReadImage
ReadImage::ReadImage(const GlobalVar& image,
                     const SamplerVar& sampler,
                     const IValue& x,
                     const IValue& y)
    : _image(image),
      _sampler(sampler),
      _x(x),
      _y(y) { }

string ReadImage::name(void) const
{
    stringstream ss;

    switch (_image.precision())
    {
        case (PrecType::UInt32) :
            ss << "read_imageui(";
            break;

        case (PrecType::Int32) :
            ss << "read_imagei(";
            break;

        case (PrecType::Float) :
            ss << "read_imagef(";
            break;

        case (PrecType::Double) :
            ss << "read_imageui(";
            break;
    }

    ss << _image.name()
       << ", "
       << _sampler.name()
       << ", (int2)(" << _x.name() << ", " << _y.name() << "))";

    return ss.str();
}

// WriteImage
WriteImage::WriteImage(const GlobalVar& image,
                       const IValue& x,
                       const IValue& y,
                       const IValue& value,
                       Indent& indent)
    : IPrintable(indent),
      _image(image),
      _x(x),
      _y(y),
      _value(value) { }

ostream& WriteImage::print(ostream& os) const
{
    os << _indent;

    switch (_image.precision())
    {
        case (PrecType::UInt32) :
            os << "write_imageui(";
            break;

        case (PrecType::Int32) :
            os << "write_imagei(";
            break;

        case (PrecType::Float) :
            os << "write_imagef(";
            break;

        case (PrecType::Double) :
            os << "write_imageui(";
            break;
    }

    os << _image.name() << ", (int2)("
       << _x.name() << ", "
       << _y.name() << "), "
       << _value.name() << ")";

    return endline(os);
}

}; // namespace chai_internal
