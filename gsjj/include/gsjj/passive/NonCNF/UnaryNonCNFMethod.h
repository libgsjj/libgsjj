#pragma once

#include "gsjj/passive/NonCNF/SATNonCNFMethod.h"
#include "gsjj/passive/MethodFactory.h"

namespace gsjj {
    namespace passive {
        namespace NonCNF {
            /**
             * The Grinchtein, Leucker and Piterman's unary method.
             * 
             * It relies on Non CNF SAT solver
             */
            class UnaryNonCNFMethod : public SATNonCNFMethod, public RegisterInFactory<UnaryNonCNFMethod> {
            public:
                UnaryNonCNFMethod() = delete;
                ~UnaryNonCNFMethod();

                static std::string getFactoryName();

            protected:
                virtual bool createFormula(std::ostream &stream) override;
                std::unique_ptr<DFA<char>> toDFA(std::istream &stream) const override;
                virtual std::string getFileName() const override;

                UnaryNonCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n);
            };
        }
    }
}