#include "passive/CNF/SATCNFMethod.h"

#include <iostream>

#include "utils/System.h"

using namespace Minisat;

namespace gsjj {
    namespace passive {
        namespace CNF {
            SATCNFMethod::SATCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n) :
                Method(SpSet, SmSet, SSet, prefixesSet, alphabetSet, n)
            {
            }

            SATCNFMethod::~SATCNFMethod() {

            }

            bool SATCNFMethod::solve() {
                m_solver = std::make_unique<SimpSolver>(*mustStop);
                m_solver->verbosity = 0;
                createVariables();
                createClauses();

                m_triedSolve = true;
                if (*mustStop) {
                    return false;
                }
                m_cpuTimeStart = cpuTime();
                m_hasSolution = m_solver->solve(true, false);
                m_cpuTimeEnd = cpuTime();
                return m_hasSolution;
            }

            bool SATCNFMethod::hasSolution() const {
                return m_hasSolution;
            }

            std::unique_ptr<DFA<char>> SATCNFMethod::constructDFA() {
                // We try to solve the SAT problem if not yet done
                if (!m_triedSolve) {
                    if (!solve()) {
                        return nullptr;
                    }
                }
                // If the SAT problem is not satisfiable
                if (!hasSolution()) {
                    return nullptr;
                }
                // The SAT problem has a solution
                else {
                    return toDFA(m_solver->model);
                }
            }

            void SATCNFMethod::printVariables() const {
                if (!m_triedSolve) {
                    std::cout << "NOT SOLVED\n";
                }
                else if (!hasSolution()) {
                    std::cout << "UNSAT\n";
                }
                else {
                    std::cout << "SAT\n";
                    for (int i = 0 ; i < m_solver->model.size() ; i++) {
                        std::cout << " " << (m_solver->model[i] == l_True ? "" : "-") << i;
                    }
                    std::cout << "\n";
                }
            }

            void SATCNFMethod::setStopTrigger(const std::chrono::seconds &timeLimit, std::atomic_bool &stopTrigger, const bool *stopPointer) {
                mustStop = &stopTrigger;
            }

            Minisat::Var SATCNFMethod::newVariable() {
                return m_solver->newVar();
            }

            void SATCNFMethod::addClause(const Minisat::vec<Minisat::Lit> &clause) {
                m_solver->addClause(clause);
            }
        }
    }
}