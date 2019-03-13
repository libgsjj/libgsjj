/**
 * \file HeuleVerwerNonCNFMethod.h
 * 
 * This file declares the Heule and Verwer method that relies on non CNF SAT solver.
 */

#pragma once

#include "passive/NonCNF/SATNonCNFMethod.h"

namespace passive {
    namespace NonCNF {
        /**
         * The Heule and Verwer method.
         */
        class HeuleVerwerNonCNFMethod : public SATNonCNFMethod {
        public:
            HeuleVerwerNonCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n, const bool *stopTrigger);
            ~HeuleVerwerNonCNFMethod();

        protected:
            virtual bool createFormula(std::ostream &stream) override;
            // void createVariables() override;
            // void createClauses() override;
            std::unique_ptr<DFA<char>> toDFA(std::istream &stream) const override;
            virtual std::string getFileName() const override;
        };
    }
}