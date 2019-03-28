#include "gsjj/passive/CNF/SATCNFMethod.h"

#include <iostream>

#include "MapleCOMSPS_LRB/utils/System.h"

using namespace Minisat;

namespace gsjj {
    namespace passive {
        namespace CNF {
            SATCNFMethod::SATCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n) :
                Method(SpSet, SmSet, SSet, prefixesSet, alphabetSet, n),
                mustStop(nullptr)
            {
            }

            SATCNFMethod::~SATCNFMethod() {

            }

            bool SATCNFMethod::solve() {
                if (mustStop) {
                    m_solver = std::make_unique<SimpSolver>(*mustStop);
                }
                else {
                    std::atomic_bool falseBool(false);
                    m_solver = std::make_unique<SimpSolver>(falseBool);
                }
                m_solver->verbosity = 0;
                createVariables();
                createClauses();

                m_triedSolve = true;
                // If mustStop is a valid pointer and if the value is true
                if (mustStop && *mustStop) {
                    return false;
                }
                m_cpuTimeStart = cpuTime();
                m_hasSolution = m_solver->solve(true, false);
                m_cpuTimeEnd = cpuTime();
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