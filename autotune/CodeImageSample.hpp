// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_CODE_IMAGE_SAMPLE_HPP_
#define _CHAI_CODE_IMAGE_SAMPLE_HPP_

#include <ostream>
#include <string>

#include "CodeFormat.hpp"
#include "CodeInterfaces.hpp"
#include "CodeVars.hpp"

namespace chai_internal {

////////////////////////////////////////
// images and texture sampling

// required sampler settings for 2D images
// use like this: SampleVar sampler;
//                os << declare(sampler, ImageSampler());

class SamplerVar : public IVariable
{
public:
    std::string name(void) const;
    std::string declaredName(void) const;
};

struct ImageSampler : public IValue
{
    std::string name(void) const;
};

// read float and uint32 quad from a 2D image
class ReadImage : public IValue
{
    const GlobalVar&  _image;
    const SamplerVar& _sampler;
    const IValue&     _x;
    const IValue&     _y;

public:
    ReadImage(const GlobalVar& image,
              const SamplerVar& sampler,
              const IValue& x,
              const IValue& y);

    std::string name(void) const;
};

// write float and uint32 quad to a 2D image
class WriteImage : public IPrintable
{
    const GlobalVar& _image;
    const IValue&    _x;
    const IValue&    _y;
    const IValue&    _value;

public:
    WriteImage(const GlobalVar& image,
               const IValue& x,
               const IValue& y,
               const IValue& value,
               Indent& indent = Indent::defaultObj());

    std::ostream& print(std::ostream& os) const;
};

}; // namespace chai_internal

#endif
