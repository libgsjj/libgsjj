#pragma once

#include "gsjj/passive/CNF/SATCNFMethod.h"
#include "gsjj/passive/MethodFactory.h"

namespace gsjj {
    namespace passive {
        namespace CNF {
            /**
             * The Grinchtein, Leucker and Piterman's unary method.
             * 
             * It relies on SAT solver
             */
            class UnaryCNFMethod : public SATCNFMethod, public RegisterInFactory<UnaryCNFMethod> {
            public:
                UnaryCNFMethod() = delete;
                ~UnaryCNFMethod();

                static std::string getFactoryName();

            protected:
                void createVariables() override;
                void createClauses() override;
                std::unique_ptr<DFA<char>> toDFA(const Minisat::vec<Minisat::lbool> &model) const override;

                UnaryCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n);

            private:
                /**
                 * The type of the map to use to know the Variable (in SAT) in function of (the prefix, the state in the DFA)
                 */
                typedef std::map<std::pair<std::string, unsigned int>, Minisat::Var> states_map;
                /**
                 * A shortcut for std::pair<std::pair<std::string, unsigned int>, Var>
                 */
                typedef states_map::value_type state;

            private:
                states_map m_stateName;
            };
        }
    }
}