/**
 * \file HeuleVerwerCNFMethod.cpp
 * 
 * This file implements the Heule and Verwer method that relies on CNF SAT solver.
 */

#include "passive/CNF/HeuleVerwerCNFMethod.h"

#include <iostream>

using namespace Minisat;

namespace gsjj {
    namespace passive {
        namespace CNF {
            HeuleVerwerCNFMethod::HeuleVerwerCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n, std::atomic_bool &stopTrigger) :
                SATCNFMethod(SpSet, SmSet, SSet, prefixesSet, alphabetSet, n, stopTrigger)
            {
            }

            HeuleVerwerCNFMethod::~HeuleVerwerCNFMethod() {
            }

            void HeuleVerwerCNFMethod::createVariables() {
                // The x variables (same as for Grinchtein, Leucker and Piterman's unary method)
                for (const auto &u : m_prefixes) {
                    for (unsigned int q = 0; q < m_numberStates; q++) {
                        auto pair = std::make_pair(u, q);
                        Var var = newVariable();
                        m_xVars.emplace(pair, var);
                    }
                }

                // The d variables : for each state, for each symbol, for each state, there is a distinct variable
                for (unsigned int p = 0; p < m_numberStates; p++) {
                    for (const char &a : m_alphabet) {
                        for (unsigned int q = 0; q < m_numberStates; q++) {
                            auto trip = std::make_tuple(p, a, q);
                            Var var = newVariable();
                            m_dVars.emplace(trip, var);
                        }
                    }
                }

                // The f variables : for each state, there is a distinct variable
                for (unsigned int q = 0; q < m_numberStates; q++) {
                    m_fVars.emplace(q, newVariable());
                }
            }

            void HeuleVerwerCNFMethod::createClauses() {
                // Equation 4.5.1
                // For every p in Q, a in Sigma, q, q' in Q such that q != q', we must have NOT (d_{p, a, q} AND d_{p, a, q'})
                // In other words, d must encode a deterministic transition function
                vec<Lit> clause(2);
                for (unsigned int p = 0; p < m_numberStates; p++) {
                    for (const char &a : m_alphabet) {
                        for (unsigned int q = 0; q < m_numberStates; q++) {
                            for (unsigned int r = 0; r < m_numberStates; r++) {
                                if (q != r) {
                                    clause[0] = mkLit(m_dVars.find(std::make_tuple(p, a, q))->second, true);
                                    clause[1] = mkLit(m_dVars.find(std::make_tuple(p, a, r))->second, true);
                                    addClause(clause);
                                }
                            }
                        }
                    }
                }

                // Equation 4.5.2
                // For every u in Pref, we must have that there exists (at least) one q in Q such that x_{u, q} is true
                // In other words, every prefix must reach at least one state
                clause.clear();
                clause.growTo(m_numberStates);
                for (const auto &u : m_prefixes) {
                    for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                        clause[q] = mkLit(m_xVars.find(std::make_pair(u, q))->second, false);
                    }
                    addClause(clause);
                }

                // Equation 4.5.3
                // For every ua in Pref, p, q in Q, we must have (x_{u, p} AND d_{p, a, q}) => x_{ua, q}
                // In other words, if delta(q_0, u) = p and delta(p, a) = q, we must have delta(q_0, ua) = q
                clause.clear();
                clause.growTo(3);
                for (const auto &ua : m_prefixes) {
                    if (ua.size() > 0) {
                        const std::string &u = ua.substr(0, ua.size() - 1);
                        const char &a = ua.at(ua.size() - 1);

                        for (unsigned int p = 0 ; p < m_numberStates ; p++) {
                            for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                                clause[0] = mkLit(m_xVars.find(std::make_pair(u, p))->second, true);
                                clause[1] = mkLit(m_dVars.find(std::make_tuple(p, a, q))->second, true);
                                clause[2] = mkLit(m_xVars.find(std::make_pair(ua, q))->second, false);
                                addClause(clause);
                            }
                        }
                    }
                }
                addClause(clause);

                // Equation 4.5.4
                // For every u in S_+, q in Q, x_{u, q} => f_q
                // AND
                // For every u in S_-, q in Q, x_{u, q} => NOT f_q
                // In other words, the DFA must be consistent with S
                clause.clear();
                clause.growTo(2);
                for (unsigned int q = 0; q < m_numberStates; q++) {
                    for (const auto &u : m_Sp) {
                        clause[0] = mkLit(m_xVars.find(std::make_pair(u, q))->second, true);
                        clause[1] = mkLit(m_fVars.find(q)->second, false);
                        addClause(clause);
                    }

                    for (const auto &u : m_Sm) {
                        clause[0] = mkLit(m_xVars.find(std::make_pair(u, q))->second, true);
                        clause[1] = mkLit(m_fVars.find(q)->second, true);
                        addClause(clause);
                    }
                }

                // Equation 4.5.5
                // For every p in Q, a in Sigma, we must have that there exists (at least) on q in Q such that d_{p, a, q} is true
                // This equation forces that every state has an outgoing transition for every symbol in the alphabet
                clause.clear();
                clause.growTo(m_numberStates);
                for (unsigned int p = 0; p < m_numberStates; p++) {
                    for (const char &a : m_alphabet) {
                        for (unsigned int q = 0; q < m_numberStates; q++) {
                            clause[q] = mkLit(m_dVars.find(std::make_tuple(p, a, q))->second, false);
                        }
                        addClause(clause);
                    }
                }

                // Equation 4.5.6
                // For every u in Pref, p, q in Q such that p != q, me must have NOT (x_{u, p} AND x_{u, q})
                // This equation forces that at most one variable in x_{u, q_0}, ..., x_{u, q_n} is true
                // Therefore, along with 4.5.2, we now have that every prefix ends in exactly one state
                clause.clear();
                clause.growTo(2);
                for (const auto &u : m_prefixes) {
                    for (unsigned int p = 0 ; p < m_numberStates ; p++) {
                        for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                            if (p != q) {
                                clause[0] = mkLit(m_xVars.find(std::make_pair(u, p))->second, true);
                                clause[1] = mkLit(m_xVars.find(std::make_pair(u, q))->second, true);
                                addClause(clause);
                            }
                        }
                    }
                }

                // Equation 4.5.7
                // For every ua in Pref, p, q in Q, we must have that (x_{u, p} AND x_{ua, q}) => d_{p, a, q}
                // If we have delta(q_0, u) = p and delta(q_0, ua) = q, then we must have that delta(p, a) = q to remain coherent
                clause.clear();
                clause.growTo(3);
                for (const auto &ua : m_prefixes) {
                    if (ua.size() > 0) {
                        const std::string &u = ua.substr(0, ua.size() - 1);
                        const char &a = ua.at(ua.size() - 1);

                        for (unsigned int p = 0 ; p < m_numberStates ; p++) {
                            for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                                clause[0] = mkLit(m_xVars.find(std::make_pair(u, p))->second, true);
                                clause[1] = mkLit(m_xVars.find(std::make_pair(ua, q))->second, true);
                                clause[2] = mkLit(m_dVars.find(std::make_tuple(p, a, q))->second, false);
                                addClause(clause);
                            }
                        }
                    }
                }
            }

            std::unique_ptr<DFA<char>> HeuleVerwerCNFMethod::toDFA(const Minisat::vec<Minisat::lbool> &model) const {
                // First, we must seek the initial state
                unsigned int initial = 0;
                for (unsigned int i = 0; i < m_numberStates; i++) {
                    Var v = m_xVars.find(std::make_pair("", i))->second;
                    if (model[v] == l_True) {
                        initial = i;
                        break;
                    }
                }

                // The initial state is accepting if f_{initial} is true
                std::unique_ptr<DFA<char>> dfa = std::make_unique<DFA<char>>(initial, model[m_fVars.find(initial)->second] == l_True);

                // Now, we create every other state (and define the accepting ones)
                for (unsigned int q = 0; q < m_numberStates; q++) {
                    if (q != initial) {
                        dfa->addState(q, model[m_fVars.find(q)->second] == l_True);
                    }
                }

                // Finally, we create the transitions
                for (unsigned int p = 0; p < m_numberStates; p++) {
                    for (const char &a : m_alphabet) {
                        for (unsigned int q = 0; q < m_numberStates; q++) {
                            if (model[m_dVars.find(std::make_tuple(p, a, q))->second] == l_True) {
                                dfa->addTransition(p, a, q);
                            }
                        }
                    }
                }

                return dfa;
            }
        }
    }
}