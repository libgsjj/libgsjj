#include "gsjj/passive/SMT/NeiderJansenMethod.h"

using namespace CVC4;

bool isAccepting(Expr f_value) {
    return f_value.getConst<Rational>().getNumerator().toUnsignedInt() != 0;
}

namespace gsjj {
    namespace passive {
        namespace SMT {
            NeiderJansenMethod::NeiderJansenMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n) :
                SMTMethod(SpSet, SmSet, SSet, prefixesSet, alphabetSet, n)
            {
                s_registered = s_registered;
                LogicInfo logic;
                logic.enableIntegers();
                logic.enableTheory(theory::THEORY_UF);
                m_SMTEngine.setLogic(logic);
            }

            NeiderJansenMethod::~NeiderJansenMethod() {

            }

            std::string NeiderJansenMethod::getFactoryName() {
                return "neider";
            }

            void NeiderJansenMethod::createVariables() {
                // Basic types
                Type integer = m_exprManager.integerType();

                // Now, we create a type for the N x N domain
                DatatypeType intTimesInt = m_exprManager.mkTupleType({integer, integer});
                // The constructor for this tuple
                intTimesIntConstructor = intTimesInt.getDatatype()[0].getConstructor();

                // We can finally create the function's types needed
                // The type for f and x
                Type intToInt = m_exprManager.mkFunctionType(integer, integer);
                // The type for d
                Type intTimesIntToInt = m_exprManager.mkFunctionType(intTimesInt, integer);

                // We also create the functions
                x = m_exprManager.mkVar("x", intToInt);
                f = m_exprManager.mkVar("f", intToInt);
                d = m_exprManager.mkVar("d", intTimesIntToInt);

                // Some constants
                zero = m_exprManager.mkConst(Rational(0));
                n = m_exprManager.mkConst(Rational(m_numberStates));

                // We create a distinct constant for every prefix
                unsigned int i = 0;
                for (const auto &u : m_prefixes) {
                    m_prefixToInt.emplace(u, m_exprManager.mkConst(Rational(i++)));
                }
            }

            CVC4::Expr NeiderJansenMethod::createConstraints() {

                Expr x_0 = m_exprManager.mkExpr(kind::APPLY_UF, x, zero);

                // Equation 4.6.1 : x(0) < n
                Expr assumptions = m_exprManager.mkExpr(kind::LT, x_0, n);

                // Equation 4.6.2 : for all i in Q, a in Sigma, d(i, a) < n
                for (unsigned int i = 0 ; i < m_numberStates ; i++) {
                    for (const char &a : m_alphabet) {
                        Expr i_a = m_exprManager.mkExpr(kind::APPLY_CONSTRUCTOR, intTimesIntConstructor, m_exprManager.mkConst(Rational(i)), m_exprManager.mkConst(Rational(a)));
                        assumptions = assumptions.andExpr(
                            m_exprManager.mkExpr(
                                kind::LT,
                                m_exprManager.mkExpr(
                                    kind::APPLY_UF,
                                    d,
                                    i_a
                                ),
                                n
                            )
                        );
                    }
                }

                // Equation 4.6.3 : for all u_i, u_j in Pref, for all a in Sigma such that u_j = u_i a, x(j) = d(x(i), a)
                for (const auto &ui : m_prefixes) {
                    for (const auto &uj : m_prefixes) {
                        // We need that u_j = u_i
                        if (uj.size() > 0 && uj.substr(0, uj.size() - 1) == ui) {
                            Expr a = m_exprManager.mkConst(Rational(uj.at(uj.size() - 1)));
                            Expr i = m_prefixToInt[ui];
                            Expr j = m_prefixToInt[uj];

                            Expr x_j = m_exprManager.mkExpr(kind::APPLY_UF, x, j);
                            Expr x_i = m_exprManager.mkExpr(kind::APPLY_UF, x, i);
                            Expr x_i_a = m_exprManager.mkExpr(kind::APPLY_CONSTRUCTOR, intTimesIntConstructor, x_i, a);
                            
                            assumptions = assumptions.andExpr(
                                m_exprManager.mkExpr(kind::EQUAL,
                                    x_j,
                                    m_exprManager.mkExpr(kind::APPLY_UF, d, x_i_a)
                                )
                            );
                        }
                    }
                }

                // Equation 4.6.4
                for (const auto &ui : m_Sp) {
                    Expr x_i = m_exprManager.mkExpr(kind::APPLY_UF, x, m_prefixToInt[ui]);
                    Expr f_x_i = m_exprManager.mkExpr(kind::APPLY_UF, f, x_i);
                    assumptions = assumptions.andExpr(m_exprManager.mkExpr(kind::DISTINCT, f_x_i, zero));
                }
                for (const auto &ui : m_Sm) {
                    Expr x_i = m_exprManager.mkExpr(kind::APPLY_UF, x, m_prefixToInt[ui]);
                    Expr f_x_i = m_exprManager.mkExpr(kind::APPLY_UF, f, x_i);
                    assumptions = assumptions.andExpr(m_exprManager.mkExpr(kind::EQUAL, f_x_i, zero));
                }

                // Finally, we must have that x(i) >= 0 (because it's a N -> N function)
                for (const auto &ui : m_prefixes) {
                    Expr x_i = m_exprManager.mkExpr(kind::APPLY_UF, x, m_prefixToInt[ui]);
                    assumptions = assumptions.andExpr(m_exprManager.mkExpr(kind::GEQ, x_i, zero));
                }

                return assumptions;
            }

            std::unique_ptr<DFA<char>> NeiderJansenMethod::toDFA() {
                // We start by searching the initial state
                Expr x_0 = m_exprManager.mkExpr(kind::APPLY_UF, x, zero);
                unsigned int initial = m_SMTEngine.getValue(x_0).getConst<Rational>().getNumerator().toUnsignedInt();

                std::unique_ptr<DFA<char>> dfa = std::make_unique<DFA<char>>(initial, isAccepting(m_SMTEngine.getValue(m_exprManager.mkExpr(kind::APPLY_UF, f, x_0))));

                // Now, we process the other states
                for (unsigned int i = 0 ; i < m_numberStates ; i++) {
                    if (i != initial) {
                        Expr f_i = m_exprManager.mkExpr(kind::APPLY_UF, f, m_exprManager.mkConst(Rational(i)));

                        dfa->addState(i, isAccepting(m_SMTEngine.getValue(f_i)));
                    }
                }

                // Finally, we create the transitions
                for (unsigned int i = 0 ; i < m_numberStates ; i++) {
                    for (const char &a : m_alphabet) {
                        Expr i_a = m_exprManager.mkExpr(kind::APPLY_CONSTRUCTOR, intTimesIntConstructor, m_exprManager.mkConst(Rational(i)), m_exprManager.mkConst(Rational(a)));
                        Expr d_i_a = m_exprManager.mkExpr(kind::APPLY_UF, d, i_a);

                        unsigned int j = m_SMTEngine.getValue(d_i_a).getConst<Rational>().getNumerator().toUnsignedInt();

                        dfa->addTransition(i, a, j);
                    }
                }

                return dfa;
            }
        }
    }
}