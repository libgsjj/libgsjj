#include "passive/SMT/BiermannMethod.h"

using namespace CVC4;

namespace gsjj {
    namespace passive {
        namespace SMT {
            BiermannMethod::BiermannMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n, const std::chrono::seconds &timeLimit) :
                SMTMethod(SpSet, SmSet, SSet, prefixesSet, alphabetSet, n, timeLimit)
            {

            }

            BiermannMethod::~BiermannMethod() {

            }

            void BiermannMethod::createVariables() {
                Type integer = m_exprManager.integerType();

                for (const auto &u : m_prefixes) {
                    Expr x = m_exprManager.mkVar(u, integer);
                    m_stateName[u] = x;
                }
            }

            CVC4::Expr BiermannMethod::createConstraints() {
                Expr formula;
                
                // Values in [0, numberStates-1]
                Expr zero = m_exprManager.mkConst(Rational(0));
                Expr n = m_exprManager.mkConst(Rational(m_numberStates - 1));
                for (const auto &u : m_prefixes) {
                    Expr positive = m_exprManager.mkExpr(kind::GEQ, m_stateName[u], zero);
                    Expr lowerN = m_exprManager.mkExpr(kind::LEQ, m_stateName[u], n);
                    Expr between = positive.andExpr(lowerN);
                    if (formula.isNull()) {
                        formula = between;
                    }
                    else {
                        formula = formula.andExpr(between);
                    }
                }

                // Equation 4.2.1
                for (const auto &ua : m_prefixes) {
                    if (ua.size() > 0) {
                        const char &a = ua.at(ua.size() - 1);
                        const std::string &u = ua.substr(0, ua.size() - 1);
                        for (const auto &va : m_prefixes) {
                            if(va.size() > 0 && va.at(va.size() - 1) == a) {
                                const std::string &v = va.substr(0, va.size() - 1);

                                Expr premise = m_exprManager.mkExpr(kind::EQUAL, m_stateName[u], m_stateName[v]);
                                Expr conclusion = m_exprManager.mkExpr(kind::EQUAL, m_stateName[ua], m_stateName[va]);

                                Expr implication = m_exprManager.mkExpr(kind::IMPLIES, premise, conclusion);

                                formula = formula.andExpr(implication);
                            }
                        }
                    }
                }

                // Equation 4.2.2
                for (const auto &u : m_Sp) {
                    for (const auto &v : m_Sm) {
                        Expr diff = m_exprManager.mkExpr(kind::EQUAL, m_stateName[u], m_stateName[v]).notExpr();

                        formula = formula.andExpr(diff);
                    }
                }

                return formula;
            }

            std::unique_ptr<DFA<char>> BiermannMethod::toDFA() {
                // First, we must seek the initial state
                unsigned int initial = m_SMTEngine.getValue(m_stateName.find("")->second).getConst<Rational>().getNumerator().toUnsignedInt();

                std::unique_ptr<DFA<char>> dfa = std::make_unique<DFA<char>>(initial, m_Sp.find("") != m_Sp.end());

                // Now, we create every needed state (and we set it accepting if needed)
                for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                    if (q != initial) {
                        bool accepting = false;
                        for (const auto &u : m_Sp) {
                            Expr expr = m_stateName.find(u)->second;
                            if (m_SMTEngine.getValue(expr).getConst<Rational>().getNumerator().toUnsignedInt() == q) {
                                accepting = true;
                                break;
                            }
                        }

                        dfa->addState(q, accepting);
                    }
                }

                // Finally, we create delta
                for (unsigned int i = 0 ; i < m_numberStates ; i++) {
                    for (const char &a : m_alphabet) {
                        bool found = false;
                        for (unsigned int j = 0 ; j < m_numberStates ; j++) {
                            for (const auto &ua : m_prefixes) {
                                if (ua.size() > 0 && ua.at(ua.size() - 1) == a){
                                    std::string u = ua.substr(0, ua.size() - 1);

                                    Expr uState = m_stateName.find(u)->second;
                                    Expr uaState = m_stateName.find(ua)->second;

                                    if (m_SMTEngine.getValue(uState).getConst<Rational>().getNumerator().toUnsignedInt() == i && m_SMTEngine.getValue(uaState).getConst<Rational>().getNumerator().toUnsignedInt() == j) {
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