/**
 * \file HeuleVerwerCNFMethod.h
 * 
 * This file declares the Heule and Verwer method that relies on CNF SAT solver.
 */

#pragma once

#include "gsjj/passive/CNF/SATCNFMethod.h"
#include "gsjj/passive/MethodFactory.h"

namespace gsjj {
    namespace passive {
        namespace CNF {
            /**
             * The Heule and Verwer method.
             */
            class HeuleVerwerCNFMethod : public SATCNFMethod, public RegisterInFactory<HeuleVerwerCNFMethod> {
            public:
                HeuleVerwerCNFMethod() = delete;
                ~HeuleVerwerCNFMethod();

                static std::string getFactoryName();

            protected:
                std::map<std::pair<std::string, unsigned int>, Minisat::Var> m_xVars;
                std::map<std::tuple<unsigned int, char, unsigned int>, Minisat::Var> m_dVars;
                std::map<unsigned int, Minisat::Var> m_fVars;

            protected:
                void createVariables() override;
                void createClauses() override;
                std::unique_ptr<DFA<char>> toDFA(const Minisat::vec<Minisat::lbool> &model) const override;

                HeuleVerwerCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n);
            };
        }
    }
}