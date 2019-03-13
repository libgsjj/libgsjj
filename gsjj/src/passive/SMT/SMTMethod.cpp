#include "passive/SMT/SMTMethod.h"

#include "utils/System.h"

namespace passive {
    namespace SMT {
        SMTMethod::SMTMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n, const std::chrono::seconds &timeLimit) :
            Method(SpSet, SmSet, SSet, prefixesSet, alphabetSet, n),
            m_exprManager(),
            m_SMTEngine(&m_exprManager)
        {
            // To enable the .getValue() function
            m_SMTEngine.setOption("produce-models", true);

            m_SMTEngine.setTimeLimit(std::chrono::milliseconds(timeLimit).count());
        }

        SMTMethod::~SMTMethod() {
            
        }

        bool SMTMethod::solve() {
            createVariables();
            CVC4::Expr expr = createConstraints();

            m_triedSolve = true;
            m_cpuTimeStart = Minisat::cpuTime();
            CVC4::Result result = m_SMTEngine.checkSat(expr);
            m_cpuTimeEnd = Minisat::cpuTime();
            m_hasSolution = result == CVC4::Result::SAT;
            return m_hasSolution;
        }

        bool SMTMethod::hasSolution() const {
            return m_hasSolution;
        }

        std::unique_ptr<DFA<char>> SMTMethod::constructDFA() {
            if (!m_triedSolve) {
                solve();
            }
            if (m_hasSolution) {
                return toDFA();
            }
            return nullptr;
        }

        void SMTMethod::printVariables() const {
            std::cout << "NOT YET IMPLEMENTED; you cannot print the variables of a SMT method for the moment\n";
        }
    }
}