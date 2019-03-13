#pragma once

#include <map>

#include <cvc4/cvc4.h>

#include "passive/Method.h"

namespace gsjj {
    namespace passive {
        /**
         * Contains every method that relies on a SMT solver.
         * 
         * These methodes use CVC4.
         */
        namespace SMT {
            /**
             * The base class for every SMT-based method.
             */
            class SMTMethod : public Method {
            public:
                SMTMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n, const std::chrono::seconds &timeLimit);
                virtual ~SMTMethod();

                bool solve() override;

                bool hasSolution() const override;

                std::unique_ptr<DFA<char>> constructDFA() override;

                void printVariables() const override;

            protected:
                /**
                 * This must be used to create new variables, constants and expressions
                 */
                CVC4::ExprManager m_exprManager;
                /**
                 * This computes acceptable values for the variables and allows to retrieve them
                 */
                CVC4::SmtEngine m_SMTEngine;

            protected:
                /**
                 * Creates all needed variables for the method
                 */
                virtual void createVariables() = 0;
                /**
                 * Creates an unique expression describing the constraints of this method.
                 * @return The expression
                 */
                virtual CVC4::Expr createConstraints() = 0;
                /**
                 * Effectively creates the DFA from the values of the variables in the SMT solver (see SMTMethod::m_SMTEngine)
                 * @return An unique_ptr to the constructed DFA
                 */
                virtual std::unique_ptr<DFA<char>> toDFA() = 0;

            private:
                bool m_hasSolution;
            };
        }
    }
}