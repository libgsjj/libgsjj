#pragma once

#include <vector>

#include "gsjj/passive/NonCNF/SATNonCNFMethod.h"
#include "gsjj/passive/MethodFactory.h"

namespace gsjj {
    namespace passive {
        namespace NonCNF {
            /**
             * The Grinchtein, Leucker and Piterman's binary method using a SAT Solver that does not rely on CNF
             */
            class BinaryNonCNFMethod : public SATNonCNFMethod, public RegisterInFactory<BinaryNonCNFMethod> {
            public:
                BinaryNonCNFMethod() = delete;
                ~BinaryNonCNFMethod();

                static std::string getFactoryName();

            protected:
                virtual bool createFormula(std::ostream &stream) override;
                virtual std::unique_ptr<DFA<char>> toDFA(std::istream &stream) const override;
                virtual std::string getFileName() const override;

                BinaryNonCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n);

            private:
                std::string phi_different(std::string pref1, std::string pref2);
                std::string phi_not_q_i(std::string pref, unsigned int i);
                /**
                 * The type of the map to use to know the Variable (in SAT) in function of (the prefix, the state in the DFA)
                 */
                typedef std::map<std::pair<std::string, unsigned int>, unsigned int> states_map;
                /**
                 * A shortcut for std::pair<std::string, unsigned int>
                 */
                typedef states_map::value_type state;
            };
        }
    }
}