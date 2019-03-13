/**
 * \file BinaryCNFMethod.h
 * 
 * This file declares the Biermann method that relies on a CNF SAT solver.
 */

#pragma once

#include "bc.hh"
 
#include "passive/Method.h"

namespace gsjj {
    namespace passive {
        namespace CNF {
            /**
             * The binary method with a CNF SAT solver.
             * 
             * Since we need to use a tool to translate the boolean formula in CNF, this implementation is different from the other methods relying on CNF Solvers.
             */
            class BinaryCNFMethod : public Method {
            public:
                BinaryCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n, std::atomic_bool &stopTrigger);
                virtual ~BinaryCNFMethod();

                bool solve() override;

                bool hasSolution() const override;

                std::unique_ptr<DFA<char>> constructDFA() override;

                void printVariables() const override;

            private:
                typedef std::map<std::pair<std::string, unsigned int>, Gate*> m_mapType;

            private:
                bool m_hasSolution;
                BC m_solver;
                SimplifyOptions m_simplify_opts;
                /**
                 * \f$m = log_2(n)\f$
                 */
                unsigned int m_binarySize;
                m_mapType m_stateToVar;

            private:
                void createVariables();
                void createFormula();

                Gate* equal(const std::string &u, const std::string &v);
                Gate* phi_different(const std::string &u, const std::string &v);
                Gate* phi_not_q_i(const std::string &u, unsigned int i);

                unsigned int toState(const std::string &u) const;

                std::unique_ptr<DFA<char>> toDFA() const;
            };
        }
    }
}