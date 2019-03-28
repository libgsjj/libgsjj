/**
 * \file BinaryCNFMethod.cpp
 * 
 * This file implements the Biermann method that relies on CNF SAT solver.
 */

#include "gsjj/passive/CNF/BinaryCNFMethod.h"

#include <iostream>
#include <cmath>

#include "MapleCOMSPS_LRB/utils/System.h"

#include "gsjj/binary.h"

using namespace Minisat;

namespace gsjj {
    namespace passive {
        namespace CNF {
            BinaryCNFMethod::BinaryCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n) :
                Method(SpSet, SmSet, SSet, prefixesSet, alphabetSet, n)
            {
                // Defaults values for the options (taken from the bcsat code)
                m_simplify_opts.constant_folding = true;
                m_simplify_opts.downward_bcp = true;
                m_simplify_opts.remove_duplicate_children = true;
                m_simplify_opts.remove_g_not_g_children = true;
                m_simplify_opts.inline_equivalences = true;
                m_simplify_opts.misc_reductions = true;
                m_simplify_opts.use_coi = true;
                m_simplify_opts.absorb_children = SimplifyOptions::CHILDABSORB_NONE;
                verbose = false;

                m_binarySize = std::ceil(std::log2(n));

                s_registered = s_registered;
            }

            BinaryCNFMethod::~BinaryCNFMethod() {

            }

            bool BinaryCNFMethod::solve() {
                // We explicitly handles the case where n = 1 <=> m = 0
                if (m_numberStates == 1) {
                    return m_hasSolution = (m_Sp.size() == 0 || m_Sm.size() == 0);
                }

                createVariables();
                m_solver->force_true(createFormula());

                m_triedSolve = true;
                m_cpuTimeStart = cpuTime();
                // minisat_solver returns 1 iff the formula is sat
                m_hasSolution = m_solver->minisat_solve(false, m_simplify_opts, false, false, false, false, 0) == 1;
                m_cpuTimeEnd = cpuTime();
                return m_hasSolution;
            }

            std::unique_ptr<DFA<char>> BinaryCNFMethod::constructDFA() {
                // First, we explicitly handles the case where n = 1
                if (m_numberStates == 1) {
                    auto dfa = std::make_unique<DFA<char>>(0, m_Sp.size() != 0);
                    for (const char &a : m_alphabet) {
                        dfa->addTransition(0, a, 0);
                    }
                    return dfa;
                }

                // We try to solve if it is not yet done
                if (!m_triedSolve) {
                    if (!solve()) {
                        return nullptr;
                    }
                }

                // If the formula is not satisfiable, we return a null pointer
                if (!hasSolution()) {
                    return nullptr;
                }
                else {
                    return toDFA();
                }
            }

            std::string BinaryCNFMethod::getFactoryName() {
                return "binary";
            }

            void BinaryCNFMethod::setStopTrigger(const std::chrono::seconds &timeLimit, std::atomic_bool &stopTrigger, const bool *stopPointer) {
                m_solver = std::make_unique<BC>(stopTrigger);
            }

            void BinaryCNFMethod::createVariables() {
                for (const auto &u : m_prefixes) {
                    for (unsigned int i = 0 ; i < m_binarySize ; i++) {
                        Gate *var = m_solver->new_VAR();
                        m_stateToVar.emplace(std::make_pair(u, i), var);
                    }
                }
            }

            Gate* BinaryCNFMethod::createFormula() {
                Gate *formula = nullptr;

                // For every ua, va in prefixes such that the last letter of ua is the same as the last letter of va
                for (const auto &ua : m_prefixes) {
                    if (ua.size() > 0) {
                        const char &a = ua.at(ua.size() - 1);
                        const std::string &u = ua.substr(0, ua.size() - 1);

                        for (const auto &va : m_prefixes) {
                            if (va.size() > 0 && va.at(va.size() - 1) == a) {
                                const std::string &v = va.substr(0, va.size() - 1);
                                // We must force that x_u = x_v => x_{ua} = x_{va}
                                // In other words, we force that the transition function is correct and coherent
                                // So, we create a formula for NOT (x_u = x_v) OR x_ua = x_va
                                Gate *different = phi_different(u, v);
                                Gate *eq = equal(ua, va);
                                Gate *full = m_solver->new_OR(different, eq);

                                if (formula) {
                                    formula = m_solver->new_AND(formula, full);
                                }
                                else {
                                    formula = full;
                                }
                            }
                        }
                    }
                }

                // Now, we want to express that for every u in Sp and every v in Sm, x_u is different than x_v
                // In other words, u and v can not end in the same state (since u must be accepted and v rejected)
                for (const auto &u : m_Sp) {
                    for (const auto &v : m_Sm) {
                        formula = m_solver->new_AND(formula, phi_different(u, v));
                    }
                }

                // Finally, we must make sure that the encoding is correct even if the number of states is not a power of 2
                // http://www.graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
                for (const auto &u : m_prefixes) {
                    for (unsigned int i = m_numberStates ; i < pow(2, m_binarySize) ; i++) {
                        formula = m_solver->new_AND(formula, phi_not_q_i(u, i));
                    }
                }

                return formula;
            }

            Gate* BinaryCNFMethod::equal(const std::string &u, const std::string &v) {
                return m_solver->new_NOT(phi_different(u, v));
            }

            Gate* BinaryCNFMethod::phi_different(const std::string &u, const std::string &v) {
                Gate *gate = nullptr;

                for (unsigned int j = 0 ; j < m_binarySize ; j++) {
                    Gate *x_uj = m_stateToVar.find(std::make_pair(u, j))->second;
                    Gate *x_vj = m_stateToVar.find(std::make_pair(v, j))->second;
                    Gate *left = m_solver->new_AND(x_uj, m_solver->new_NOT(x_vj));
                    Gate *right = m_solver->new_AND(m_solver->new_NOT(x_uj), x_vj);
                    Gate *total = m_solver->new_OR(left, right);
                    // If gate already exists, we just add an OR operation
                    if (gate) {
                        gate = m_solver->new_OR(gate, total);
                    }
                    else {
                        gate = total;
                    }
                }
                
                return gate;
            }

            Gate* BinaryCNFMethod::phi_not_q_i(const std::string &u, unsigned int i) {
                Gate *gate = nullptr;

                std::vector<bool> bin_i = decimal_to_binary(i, m_binarySize);

                for (unsigned int j = 0 ; j < m_binarySize ; j++) {
                    Gate *x_uj = m_stateToVar.find(std::make_pair(u, j))->second;
                    Gate *l_uj = x_uj;

                    if (bin_i.at(j)) {
                        l_uj = m_solver->new_NOT(x_uj);
                    }

                    if (gate) {
                        gate = m_solver->new_OR(gate, l_uj);
                    }
                    else {
                        gate = l_uj;
                    }
                }

                return gate;
            }

            unsigned int BinaryCNFMethod::toState(const std::string &u) const {
                unsigned int state = 0;
                for (int j = m_binarySize - 1 ; j >= 0 ; j--) {
                    state += std::pow(2, j) * m_stateToVar.at(std::make_pair(u, j))->value;
                }
                return state;
            }

            std::unique_ptr<DFA<char>> BinaryCNFMethod::toDFA() const {
                // First, we seek the initial state
                unsigned int initial = toState("");

                std::unique_ptr<DFA<char>> dfa = std::make_unique<DFA<char>>(initial, m_Sp.find("") != m_Sp.end());

                // Now, we create the other states
                for (unsigned int i = 0 ; i < m_numberStates ; i++) {
                    if (i != initial) {
                        bool accepting = false;

                        // The state is accepting if there exists a word u in Sp that ends in the state u
                        for (const auto &u : m_Sp) {
                            if (i == toState(u)) {
                                accepting = true;
                                break;
                            }
                        }

                        dfa->addState(i, accepting);
                    }
                }

                // Finally, we create the transitions
                for (unsigned int i = 0 ; i < m_numberStates ; i++) {
                    for (const char &a : m_alphabet) {
                        bool found = false;
                        for (unsigned int j = 0 ; j < m_numberStates ; j++) {
                            for (const auto &ua : m_prefixes) {
                                if (ua.size() > 0 && ua.at(ua.size() - 1) == a){
                                    std::string u = ua.substr(0, ua.size() - 1);

                                    if (i == toState(u) && j == toState(ua)) {
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