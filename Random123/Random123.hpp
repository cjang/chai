// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_RANDOM123_HPP_
#define _CHAI_RANDOM123_HPP_

#include <ostream>
#include <set>
#include <string>

namespace chai_internal {

////////////////////////////////////////
// Random123 OpenCL for the JIT

class Random123
{
    const std::string _M_RAN_INVM32;
    const std::string _M_RAN_INVM64;

    std::set< size_t > _precSet;
    std::set< size_t > _vlenSet;

    bool _philox;
    bool _threefry;

    // integer typedefs

    void typedefDecl(std::ostream& os) const;

    // array

    void arrayDecl(std::ostream& os,
                   const size_t prec,
                   const size_t vlen) const;

    void arrayDecl(std::ostream& os) const;

    // philox

    void philoxMulhilo(std::ostream& os,
                       const size_t prec) const;

    void philoxBumpkey(std::ostream& os,
                       const size_t prec,
                       const size_t vlen) const;

    void philoxRoundFun(std::ostream& os,
                        const size_t prec,
                        const size_t vlen) const;

    void philoxRoundEnum(std::ostream& os,
                         const size_t prec,
                         const size_t vlen) const;

    void philoxTypedef(std::ostream& os,
                       const size_t prec,
                       const size_t vlen) const;

    void philoxKeyinit(std::ostream& os,
                       const size_t prec,
                       const size_t vlen) const;

    void philox_R(std::ostream& os,
                  const size_t prec,
                  const size_t vlen) const;

    void philoxEntry(std::ostream& os,
                     const size_t prec,
                     const size_t vlen) const;

    void philoxDecl(std::ostream& os) const;

    // threefry

    void threefryEnum(std::ostream& os,
                      const size_t prec,
                      const size_t vlen) const;

    void threefryRotL(std::ostream& os,
                      const size_t prec) const;

    void threefryTypedef(std::ostream& os,
                         const size_t prec,
                         const size_t vlen) const;

    void threefryKeyinit(std::ostream& os,
                         const size_t prec,
                         const size_t vlen) const;

    void threefry_R(std::ostream& os,
                    const size_t prec,
                    const size_t vlen) const;

    void threefryRoundEnum(std::ostream& os,
                           const size_t prec,
                           const size_t vlen) const;

    void threefryEntry(std::ostream& os,
                       const size_t prec,
                       const size_t vlen) const;

    void threefryDecl(std::ostream& os) const;

public:
    Random123(void);

    void prec32(void);
    void prec64(void);

    void vlen2(void);
    void vlen4(void);

    void philox(void);
    void threefry(void);

    void print(std::ostream&);
};

}; // namespace chai_internal

#endif
