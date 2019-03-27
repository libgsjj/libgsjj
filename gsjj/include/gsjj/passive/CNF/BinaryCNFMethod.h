/**
 * \file BinaryCNFMethod.h
 * 
 * This file declares the Biermann method that relies on a CNF SAT solver.
 */

#pragma once

#include "bcsat/bc.hh"
 
#include "gsjj/passive/Method.h"
#include "gsjj/passive/MethodFactory.h"

namespace gsjj {
    namespace passive {
        namespace CNF {
            /**
             * The binary method with a CNF SAT solver.
             * 
             * Since we need to use a tool to translate the boolean formula in CNF, this implementation is different from the other methods relying on CNF Solvers.
             */
            class BinaryCNFMethod : public Method, public RegisterInFactory<BinaryCNFMethod> {
            public:
                BinaryCNFMethod() = delete;
                virtual ~BinaryCNFMethod();

                bool solve() override;

                std::unique_ptr<DFA<char>> constructDFA() override;

                static std::string getFactoryName();

                virtual void setStopTrigger(const std::chrono::seconds &timeLimit, std::atomic_bool &stopTrigger, const bool *stopPointer) override;

            protected:
                BinaryCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n);

            private:
                typedef std::map<std::pair<std::string, unsigned int>, Gate*> m_mapType;

            private:
                bool m_hasSolution;
                std::unique_ptr<BC> m_solver;
                SimplifyOptions m_simplify_opts;
                /**
                 * \f$m = log_2(n)\f$
                 */
                unsigned int m_binarySize;
                m_mapType m_stateToVar;

            private:
                virtual void createVariables();
                virtual Gate* createFormula();

                Gate* equal(const std::string &u, const std::string &v);
                Gate* phi_different(const std::string &u, const std::string &v);
                Gate* phi_not_q_i(const std::string &u, unsigned int i);

                unsigned int toState(const std::string &u) const;

                std::unique_ptr<DFA<char>> toDFA() const;
            };
        }
    }
}