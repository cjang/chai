// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_CODE_STATEMENTS_HPP_
#define _CHAI_CODE_STATEMENTS_HPP_

#include <string>
#include <vector>

#include "CodeFormat.hpp"
#include "CodeInterfaces.hpp"
#include "CodeVars.hpp"

namespace chai_internal {

////////////////////////////////////////
// declaring variables

std::string declare(const IVariable& lhs,
                    const Indent& indent = Indent::defaultObj());

std::string declare(const IVariable& lhs,
                    const IValue& rhs,
                    const Indent& indent = Indent::defaultObj());

std::string declare(const IVariable& lhs,
                    const size_t rhs,
                    const Indent& indent = Indent::defaultObj());

std::string declare(const std::vector< PrivateVar >& lhs,
                    const Indent& indent = Indent::defaultObj());

std::string declare(const std::vector< PrivateVar >& lhs,
                    const IValue& rhs,
                    const Indent& indent = Indent::defaultObj());

std::string declare(const std::vector< PrivateVar >& lhs,
                    const size_t rhs,
                    const Indent& indent = Indent::defaultObj());

std::string declare(const std::vector< PrivateVar >& lhs,
                    const IValue& rhs,
                    const IValue& step,
                    const Indent& indent = Indent::defaultObj());

std::string declare(const std::vector< PrivateVar >& lhs,
                    const IValue& rhs,
                    const size_t step,
                    const Indent& indent = Indent::defaultObj());

std::string declare(const std::vector< std::vector< PrivateVar > >& lhs,
                    const Indent& indent = Indent::defaultObj());

std::string declare(const std::vector< std::vector< PrivateVar > >& lhs,
                    const IValue& rhs,
                    const Indent& indent = Indent::defaultObj());

std::string declare(const std::vector< std::vector< PrivateVar > >& lhs,
                    const size_t rhs,
                    const Indent& indent = Indent::defaultObj());

////////////////////////////////////////
// assigning variables

std::string assign(const IVariable& lhs,
                   const IValue& rhs,
                   const Indent& indent = Indent::defaultObj());

std::string assign(const IVariable& lhs,
                   const size_t rhs,
                   const Indent& indent = Indent::defaultObj());

std::string assign(const std::vector< PrivateVar >& lhs,
                   const IValue& rhs,
                   const Indent& indent = Indent::defaultObj());

std::string assign(const std::vector< PrivateVar >& lhs,
                   const size_t rhs,
                   const Indent& indent = Indent::defaultObj());

std::string assign(const std::vector< PrivateVar >& lhs,
                   const IValue& rhs,
                   const IValue& step,
                   const Indent& indent = Indent::defaultObj());

std::string assign(const std::vector< PrivateVar >& lhs,
                   const IValue& rhs,
                   const size_t step,
                   const Indent& indent = Indent::defaultObj());

std::string assign(const std::vector< std::vector< PrivateVar > >& lhs,
                   const IValue& rhs,
                   const Indent& indent = Indent::defaultObj());

std::string assign(const std::vector< std::vector< PrivateVar > >& lhs,
                   const size_t rhs,
                   const Indent& indent = Indent::defaultObj());

////////////////////////////////////////
// incrementing variables

std::string increment(const IVariable& lhs,
                      const IValue& rhs,
                      const Indent& indent = Indent::defaultObj());

std::string increment(const IVariable& lhs,
                      const size_t rhs,
                      const Indent& indent = Indent::defaultObj());

std::string increment(const std::vector< PrivateVar >& lhs,
                      const IValue& rhs,
                      const Indent& indent = Indent::defaultObj());

std::string increment(const std::vector< PrivateVar >& lhs,
                      const size_t rhs,
                      const Indent& indent = Indent::defaultObj());

}; // namespace chai_internal

#endif
