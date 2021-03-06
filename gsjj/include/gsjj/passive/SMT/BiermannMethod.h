#pragma once

#include "gsjj/passive/SMT/SMTMethod.h"
#include "gsjj/passive/MethodFactory.h"

namespace gsjj {
    namespace passive {
        namespace SMT {
            /**
             * Implements the Biermann and Feldman method.
             * 
             * This passive method relies on a SMT solver.
             */
            class BiermannMethod : public SMTMethod, public RegisterInFactory<BiermannMethod> {
            public:
                BiermannMethod() = delete;
                ~BiermannMethod();

                static std::string getFactoryName();

            protected:
                void createVariables() override;
                CVC4::Expr createConstraints() override;
                std::unique_ptr<DFA<char>> toDFA();

                BiermannMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n);

            private:
                typedef std::map<std::string, CVC4::Expr> statesMap;
                
            private:
                statesMap m_stateName;
            };
        }
    }
}