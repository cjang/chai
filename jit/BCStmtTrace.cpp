// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <map>
#include <set>
#include <stdint.h>
#include <vector>

#include "ByteCodes.hpp"
#include "EditStak.hpp"
#include "HashJIT.hpp"
#include "BCStmtCompound.hpp"
#include "BCStmtTrace.hpp"
#include "BCStmtRepeat.hpp"
#include "BCStmtSingle.hpp"

using namespace std;

namespace chai_internal {

size_t BCStmtTrace::liftConstants(void)
{
    map< uint32_t, size_t > varConstructorIndex;
    map< uint32_t, size_t > varDestructorIndex;
    map< uint32_t, vector< size_t > > varAssignIndex;

    for (size_t i = 0; i < _statements.size(); i++)
    {
        // all statements are really single statements until loop rolling
        BCStmtSingle* bss = static_cast< BCStmtSingle* >( _statements[i] );

        const uint32_t variable = bss->lhsVariable();
        const uint32_t version = bss->lhsVersion();

        if (0 == version)
        {
            varConstructorIndex[ variable ] = i;
        }
        else if (-1 == version)
        {
            varDestructorIndex[ variable ] = i;
        }
        else
        {
            varAssignIndex[ variable ].push_back( i );
        }
    }

    // RHS hash code -> variables
    map< uint64_t, set< uint32_t > > constantHash;
    map< uint64_t, set< uint32_t > > rngmakeHash;

    // only interested in variables without dependencies on other variables
    for (map< uint32_t, vector< size_t > >::const_iterator
         it = varAssignIndex.begin();
         it != varAssignIndex.end();
         it++)
    {
        // check if there is only one assignment to the variable
        if (1 == (*it).second.size())
        {
            BCStmtSingle* bss
                = static_cast< BCStmtSingle* >(
                      _statements[ (*it).second.front() ] );

            // hash the RHS assignment
            HashJIT v;
            bss->rhsBytecode().ascend( v );

            // must be no dependencies on other variables
            if (v.variables().empty())
            {
                // random number generation is a special case
                // what looks like the same constant function is really not
                if ( v.opCodes().count(ByteCodes::rng_normal_make_f32) ||
                     v.opCodes().count(ByteCodes::rng_normal_make_f64) ||
                     v.opCodes().count(ByteCodes::rng_uniform_make_f32) ||
                     v.opCodes().count(ByteCodes::rng_uniform_make_f64) )
                {
                    rngmakeHash[ v.code() ].insert( (*it).first );
                }
                else
                {
                    constantHash[ v.code() ].insert( (*it).first );
                }
            }
        }
    }

    // representative variables
    set< uint32_t > constantReps;

    // affected variables
    set< uint32_t > constantVars;

    // edit RHS bytecode with variable replacement (used later)
    EditStak        editVars;

    // construct map of constant variable replacement
    for (map< uint64_t, set< uint32_t > >::const_iterator
         it = constantHash.begin();
         it != constantHash.end();
         it++)
    {
        // don't care about the hash code, only needed it to map the variables

        uint32_t firstVar = -1;

        // this is the reduce part, picking a representative variable
        for (set< uint32_t >::const_iterator
             jt = (*it).second.begin();
             jt != (*it).second.end();
             jt++)
        {
            if (-1 == firstVar)
            {
                firstVar = *jt;
                constantReps.insert( *jt );
            }

            editVars.replaceVariable( *jt, firstVar );
            constantVars.insert( *jt );
        }
    }

    // representative variables
    set< uint32_t > rngmakeReps;

    // affected variables
    set< uint32_t > rngmakeVars;

    // variable replacement
    map< uint32_t, uint32_t > rngmakeMap;

    // construct map of RNG make variable replacement
    for (map< uint64_t, set< uint32_t > >::const_iterator
         it = rngmakeHash.begin();
         it != rngmakeHash.end();
         it++)
    {
        // don't care about the hash code, only needed it to map the variables

        uint32_t previous = -1;

        bool overlap = false;

        // look for special case only - contiguous disjoint lexical scopes
        for (set< uint32_t >::const_iterator
             jt = (*it).second.begin();
             jt != (*it).second.end();
             jt++)
        {
            if (-1 == previous)
            {
                previous = *jt;
            }
            else
            {
                // check for overlap in lexical scope
                if (varDestructorIndex[previous] > varConstructorIndex[*jt])
                {
                    overlap = true;
                    break;
                }
                else
                {
                    previous = *jt;
                }
            }
        }

        if (! overlap)
        {
            // all variables can be represented by one

            uint32_t firstVar = -1;

            // this is the reduce part, picking a representative variable
            for (set< uint32_t >::const_iterator
                 jt = (*it).second.begin();
                 jt != (*it).second.end();
                 jt++)
            {
                if (-1 == firstVar)
                {
                    firstVar = *jt;
                    rngmakeReps.insert( *jt );
                }

                editVars.replaceVariable( *jt, firstVar );
                rngmakeMap[ *jt ] = firstVar;
                rngmakeVars.insert( *jt );
            }
        }
    }

    // new statement vector
    vector< BCStmt* > newStatements;

    // representative constant variables go first
    for (set< uint32_t >::const_iterator
         it = constantReps.begin();
         it != constantReps.end();
         it++)
    {
        const uint32_t variable = *it;

        // constructor
        newStatements.push_back(
            _statements[ varConstructorIndex[variable] ] );

        // do not delete this statement
        _statements[ varConstructorIndex[variable] ] = NULL;

        // first assignment
        newStatements.push_back(
            _statements[ varAssignIndex[variable].front() ] );

        // do not delete this statement
        _statements[ varAssignIndex[variable].front() ] = NULL;
    }

    // next are representative RNG make variables
    for (set< uint32_t >::const_iterator
         it = rngmakeReps.begin();
         it != rngmakeReps.end();
         it++)
    {
        const uint32_t variable = *it;

        // constructor
        newStatements.push_back(
            _statements[ varConstructorIndex[variable] ] );

        // do not delete this statement
        _statements[ varConstructorIndex[variable] ] = NULL;
    }

    // append the original statements, replacing variables as we go
    for (size_t i = 0; i < _statements.size(); i++)
    {
        BCStmtSingle* bss = static_cast< BCStmtSingle* >( _statements[i] );

        if (NULL != bss)
        {
            const uint32_t variable = bss->lhsVariable();
            const uint32_t version  = bss->lhsVersion();

            // statements for affected constant variables must be skipped
            if (constantVars.count(variable))
            {
                continue;
            }

            // statements for affected RNG make variables
            if (rngmakeVars.count(variable))
            {
                // skip constructors and destructors
                if (0 == version || -1 == version)
                {
                    continue;
                }
                else
                {
                    // from earlier selection, there are no RHS variables
                    // only need to change LHS variable
                    newStatements.push_back(
                        new BCStmtSingle(i, _vt, rngmakeMap[variable]) );

                    continue;
                }
            }

            editVars.clear();

            bss->rhsBytecode().ascend(editVars);

            if (editVars.changed())
            {
                newStatements.push_back(
                    new BCStmtSingle(i, _vt, editVars.getBytecode()) );
            }
            else
            {
                newStatements.push_back( bss );

                // do not delete this statement
                _statements[i] = NULL;
            }
        }
    }

    // destructors for representative constant variables
    for (set< uint32_t >::const_iterator
         it = constantReps.begin();
         it != constantReps.end();
         it++)
    {
        const uint32_t variable = *it;

        if (varDestructorIndex.count(variable))
        {
            // destructor
            newStatements.push_back(
                _statements[ varDestructorIndex[variable] ] );

            // do not delete this statement
            _statements[ varDestructorIndex[variable] ] = NULL;
        }
    }

    // destructors for representative RNG make variables
    for (set< uint32_t >::const_iterator
         it = rngmakeReps.begin();
         it != rngmakeReps.end();
         it++)
    {
        const uint32_t variable = *it;

        if (varDestructorIndex.count(variable))
        {
            // destructor
            newStatements.push_back(
                _statements[ varDestructorIndex[variable] ] );

            // do not delete this statement
            _statements[ varDestructorIndex[variable] ] = NULL;
        }
    }

    // release unused statements
    for (vector< BCStmt* >::const_iterator
         it = _statements.begin();
         it != _statements.end();
         it++)
    {
        delete *it;
    }

    // replace statements with transformed new version
    _statements = newStatements;

    return constantReps.size() + rngmakeReps.size();
}

size_t BCStmtTrace::rollLoops(void)
{
    size_t numPasses = 0;

    while (true)
    {
        // index statement hash indexes
        map< uint64_t, vector< size_t > > hashIndexes;
        for (size_t i = 0; i < _statements.size(); i++)
        {
            const uint64_t hc = _statements[i]->hashCode();

            // hash code -> statement index vector
            hashIndexes[ hc ].push_back( i );
        }

        // hash statement gap histogram
        map< size_t, size_t >          gapCount;
        map< size_t, set< uint64_t > > gapHashes;
        for (map< uint64_t, vector< size_t > >::const_iterator
             it = hashIndexes.begin();
             it != hashIndexes.end();
             it++)
        {
            const uint64_t hc = (*it).first;

            for (size_t i = 0; i < (*it).second.size() - 1; i++)
            {
                const size_t gap = hashIndexes[hc][i+1] - hashIndexes[hc][i];

                if (0 == gapCount.count(gap))
                    gapCount[gap] = 1;
                else
                    gapCount[gap]++;

                gapHashes[gap].insert(hc);
            }
        }

        // if the gap histogram is empty, then no statements repeat
        if (gapCount.empty())
            break; // stop search

        // invert the gap histogram to find the most frequent gap size(s)
        map< size_t, set< size_t > > invertGapCount;
        for (map< size_t, size_t >::const_iterator
             it = gapCount.begin();
             it != gapCount.end();
             it++)
        {
            invertGapCount[ (*it).second ].insert( (*it).first );
        }

        bool didReplace = false;

        // iterate over largest gap sizes
        for (set< size_t >::const_iterator
             it = (*invertGapCount.rbegin()).second.begin();
             it != (*invertGapCount.rbegin()).second.end();
             it++)
        {
            const size_t           gap = *it;
            const set< uint64_t >& gapHashSet = gapHashes[gap];

            // look for the candidate repeating statements
            vector< size_t >   repeatIndexes;
            vector< uint64_t > repeatHashes;
            for (size_t i = 0; i < _statements.size(); i++)
            {
                const uint64_t hc = _statements[i]->hashCode();

                if (gapHashSet.count(hc))
                {
                    repeatIndexes.push_back( i );
                    repeatHashes.push_back( hc );
                }
                else
                {
                    if (repeatIndexes.size() < 2 * gap)
                    {
                        repeatIndexes.clear();
                        repeatHashes.clear();
                    }
                    else
                    {
                        break;
                    }
                }
            }

            // did we find anything?
            if (repeatIndexes.size() >= 2 * gap)
            {
                // check segment repetition
                size_t segRepCount = 1;
                for (size_t i = gap; i < repeatHashes.size(); i+=gap)
                {
                    bool mismatch = false;
                    for (size_t j = 0; j < gap; j++)
                    {
                        if ( repeatHashes[i + j] != repeatHashes[j] )
                        {
                            mismatch = true;
                            break;
                        }
                    }
                    if (mismatch)
                        break;
                    else
                        segRepCount++;
                }

                // the segment needs to appear at least twice to be meaningful
                if (segRepCount < 2)
                {
                    continue; // try next gap
                }

                // found a repeating segment

                // construct compound statement
                BCStmtCompound* cstmt = new BCStmtCompound;
                for (size_t i = 0; i < gap; i++)
                {
                    cstmt->push( _statements[ repeatIndexes[i] ] );

                    // do not delete this statement
                    _statements[ repeatIndexes[i] ] = NULL;
                }

                // construct repeat statement
                BCStmtRepeat* rstmt = new BCStmtRepeat( segRepCount,
                                                        cstmt );

                // replacement program statements vector
                vector< BCStmt* > newStatements;

                // before the repeating compound statement
                for (size_t i = 0; i < repeatIndexes[0]; i++)
                {
                    newStatements.push_back( _statements[i] );

                    // do not delete this statement
                    _statements[i] = NULL;
                }

                // repeating compound statement
                newStatements.push_back( rstmt );

                // after the repeating compound statement
                for (size_t i = repeatIndexes[0] + segRepCount * gap;
                     i < _statements.size();
                     i++)
                {
                    newStatements.push_back( _statements[i] );

                    // do not delete this statement
                    _statements[i] = NULL;
                }

                // release unused statements
                for (vector< BCStmt* >::const_iterator
                     it = _statements.begin();
                     it != _statements.end();
                     it++)
                {
                    delete *it;
                }

                // replace statements with transformed new version
                _statements = newStatements;

                didReplace = true;
                break;
            } // did we find anything?
        } // iterate over largest gap sizes

        if (didReplace)
            numPasses++;
        else
            break; // stop search
    }

    return numPasses;
}

void BCStmtTrace::optimizeBytecode(void)
{
    liftConstants();
    rollLoops();
}

BCStmtTrace::BCStmtTrace(VectorTrace& vt)
    : _vt(vt)
{
    for (size_t i = 0; i < vt.traceLength(); i++)
    {
        _statements.push_back( new BCStmtSingle(i, vt) );
    }
}

BCStmtTrace::~BCStmtTrace(void)
{
    for (vector< BCStmt* >::const_iterator
         it = _statements.begin();
         it != _statements.end();
         it++)
    {
        delete *it;
    }
}

VectorTrace& BCStmtTrace::getVectorTrace(void)
{
    return _vt;
}

uint64_t BCStmtTrace::hashCode(void) const
{
    return _vt.hashCode();
}

size_t BCStmtTrace::numTraces(void) const
{
    return _vt.numTraces();
}

void BCStmtTrace::traverse(VisitBCStmt& v)
{
    for (vector< BCStmt* >::const_iterator
         it = _statements.begin();
         it != _statements.end();
         it++)
    {
        (*it)->accept(v);
    }
}

}; // namespace chai_internal
