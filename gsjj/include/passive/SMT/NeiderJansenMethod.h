#pragma once

#include "passive/SMT/SMTMethod.h"

namespace gsjj {
    namespace passive {
        namespace SMT {
            /**
             * The Neider and Jansen method.
             */
            class NeiderJansenMethod : public SMTMethod {
            public:
                NeiderJansenMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n, const std::chrono::seconds &timeLimit);
                ~NeiderJansenMethod();

            protected:
                void createVariables() override;
                CVC4::Expr createConstraints() override;
                std::unique_ptr<DFA<char>> toDFA();

            private:
                typedef std::map<std::string, CVC4::Expr> prefToIntMap;
                
            private:
                std::vector<CVC4::Expr> m_variables;
                prefToIntMap m_prefixToInt;

                CVC4::Expr f, x, d;
                CVC4::Expr zero, n;
                CVC4::Expr intTimesIntConstructor;
            };
        }
    }
}