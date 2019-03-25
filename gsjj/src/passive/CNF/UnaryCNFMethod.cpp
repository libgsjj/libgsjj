#include "passive/CNF/UnaryCNFMethod.h"

#include <iostream>

using namespace Minisat;

namespace gsjj {
    namespace passive {
        namespace CNF {
            UnaryCNFMethod::UnaryCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n) :
                SATCNFMethod(SpSet, SmSet, SSet, prefixesSet, alphabetSet, n)
            {
                s_registered = s_registered;
            }

            UnaryCNFMethod::~UnaryCNFMethod() {

            }

            std::string UnaryCNFMethod::getFactoryName() {
                return "unary";
            }

            void UnaryCNFMethod::createVariables() {
                // For each prefix and each state in the DFA, we create a new variable
                for (const auto &u : m_prefixes) {
                    for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                        auto pair = std::make_pair(u, q);
                        Var var = newVariable();
                        m_stateName.insert(state(pair, var));
                    }
                }
            }

            void UnaryCNFMethod::createClauses() {
                // Equation 4.3.1
                // For some reason, Minisat defines its own vector type...
                vec<Lit> clause(m_numberStates);
                for (const auto &u : m_prefixes) {
                    for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                        // We know (u, q) is in the map thanks to createVariables
                        clause[q] = mkLit(m_stateName.find(std::make_pair(u, q))->second, false);
                    }
                    addClause(clause);
                }

                // Equation 4.3.2
                clause.clear();
                clause.growTo(2);
                for (const auto &u : m_prefixes) {
                    for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                        for (unsigned int p = 0 ; p < m_numberStates ; p++) {
                            if (p != q) {
                                Var qState = m_stateName.find(std::make_pair(u, q))->second;
                                Var pState = m_stateName.find(std::make_pair(u, p))->second;
                                clause[0] = mkLit(qState, true);
                                clause[1] = mkLit(pState, true);
                                addClause(clause);
                            }
                        }
                    }
                }

                // Equation 4.3.3
                clause.clear();
                clause.growTo(4);
                for (const auto &ua : m_prefixes) {
                    for (const auto &va : m_prefixes) {
                        // We must have that ua and va end with the same symbol
                        // We must have at least one symbol as well
                        if (ua.size() > 0 && va.size() > 0 && ua.at(ua.size() - 1) == va.at(va.size() - 1)) {
                            std::string u = ua.substr(0, ua.size() - 1);
                            std::string v = va.substr(0, va.size() - 1);
                            // Now, we can create every clause
                            for (unsigned int p = 0 ; p < m_numberStates ; p++) {
                                for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                                    Var upState = m_stateName.find(std::make_pair(u, p))->second;
                                    Var vpState = m_stateName.find(std::make_pair(v, p))->second;
                                    Var uaqState = m_stateName.find(std::make_pair(ua, q))->second;
                                    Var vaqState = m_stateName.find(std::make_pair(va, q))->second;

                                    clause[0] = mkLit(upState, true);
                                    clause[1] = mkLit(vpState, true);
                                    clause[2] = mkLit(uaqState, false);
                                    clause[3] = mkLit(vaqState, true);
                                    addClause(clause);

                                    clause[0] = mkLit(upState, true);
                                    clause[1] = mkLit(vpState, true);
                                    clause[2] = mkLit(uaqState, true);
                                    clause[3] = mkLit(vaqState, false);
                                    addClause(clause);
                                }
                            }
                        }
                    }
                }

                // Equation 4.3.4
                clause.clear();
                clause.growTo(2);
                for (const auto &u : m_Sp) {
                    for (const auto &v : m_Sm) {
                        for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                            Var uqState = m_stateName.find(std::make_pair(u, q))->second;
                            Var vqState = m_stateName.find(std::make_pair(v, q))->second;

                            clause[0] = mkLit(uqState, true);
                            clause[1] = mkLit(vqState, true);

                            addClause(clause);
                        }
                    }
                }
            }

            std::unique_ptr<DFA<char>> UnaryCNFMethod::toDFA(const Minisat::vec<Minisat::lbool> &model) const {
                // First, we seek the initial state
                unsigned int initial = 0;
                for (unsigned int i = 0 ; i < m_numberStates ; i++) {
                    Var v = m_stateName.find(std::make_pair("", i))->second;
                    if (model[v] == l_True) {
                        initial = i;
                    }
                }

                // The initial state is accepting if epsilon is in Sp
                std::unique_ptr<DFA<char>> dfa = std::make_unique<DFA<char>>(initial, m_Sp.find("") != m_Sp.end());

                // We create every state (and we seek the accepting states)
                for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                    if (q != initial) {
                        bool accepting = false;
                        for (const auto &u : m_Sp) {
                            Var var = m_stateName.find(std::make_pair(u, q))->second;
                            if (model[var] == l_True) {
                                accepting = true;
                                break;
                            }
                        }

                        dfa->addState(q, accepting);
                    }
                }

                // Now, we create delta (the transitions)
                for (unsigned int i = 0 ; i < m_numberStates ; i++) {
                    for (const char &a : m_alphabet) {
                        bool found = false;
                        for (unsigned int j = 0 ; j < m_numberStates ; j++) {
                            for (const auto &ua : m_prefixes) {
                                if (ua.size() > 0 && ua.at(ua.size() - 1) == a){
                                    std::string u = ua.substr(0, ua.size() - 1);

                                    Var uiState = m_stateName.find(std::make_pair(u, i))->second;
                                    Var uajState = m_stateName.find(std::make_pair(ua, j))->second;

                                    if (model[uiState] == l_True && model[uajState] == l_True) {
                                        found = true;
                                        dfa->addTransition(i, a, j);
                                    }
                                }
                            }
                        }

                        if (!found) {
                            dfa->addTransition(i, a, i);
                        }
                    }
                }

                return dfa;
            }
        }
    }
}