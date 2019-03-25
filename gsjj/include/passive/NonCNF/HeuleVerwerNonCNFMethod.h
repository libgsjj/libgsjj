/**
 * \file HeuleVerwerNonCNFMethod.h
 * 
 * This file declares the Heule and Verwer method that relies on non CNF SAT solver.
 */

#pragma once

#include "passive/NonCNF/SATNonCNFMethod.h"
#include "passive/MethodFactory.h"

namespace gsjj {
    namespace passive {
        namespace NonCNF {
            /**
             * The Heule and Verwer method.
             */
            class HeuleVerwerNonCNFMethod : public SATNonCNFMethod, public RegisterInFactory<HeuleVerwerNonCNFMethod> {
            public:
                HeuleVerwerNonCNFMethod() = delete;
                ~HeuleVerwerNonCNFMethod();

                static std::string getFactoryName();

            protected:
                virtual bool createFormula(std::ostream &stream) override;
                std::unique_ptr<DFA<char>> toDFA(std::istream &stream) const override;
                virtual std::string getFileName() const override;

                HeuleVerwerNonCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n);
            };
        }
    }
}