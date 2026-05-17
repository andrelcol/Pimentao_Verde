// -*-c++-*-
// Pimentao_Verde: extra dribble candidates (does not replace ActGen_ShortDribble).

#ifndef ACTGEN_IMPROVED_DRIBBLE_H
#define ACTGEN_IMPROVED_DRIBBLE_H

#include "action_generator.h"

class ActGen_ImprovedDribble
    : public ActionGenerator {
public:
    virtual
    void generate( std::vector< ActionStatePair > * result,
                   const PredictState & state,
                   const rcsc::WorldModel & wm,
                   const std::vector< ActionStatePair > & path ) const;
};

#endif
