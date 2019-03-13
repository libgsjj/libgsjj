#include "passive/NonCNF/UnaryNonCNFMethod.h"

#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>

namespace gsjj {
    namespace passive {
        namespace NonCNF {
            UnaryNonCNFMethod::UnaryNonCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n, const bool *stopTrigger) :
                SATNonCNFMethod(SpSet, SmSet, SSet, prefixesSet, alphabetSet, n, stopTrigger)
            {}

            UnaryNonCNFMethod::~UnaryNonCNFMethod() {

            }

            bool UnaryNonCNFMethod::createFormula(std::ostream &stream){
                if (m_numberStates == 1) {
                    return false;
                }
                //Equation 4.3.1
                std::string formula = "";
                for (const auto &u : m_prefixes) {
                    formula += " ( ";
                    for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                        formula += "x_" + u + "_" + std::to_string(q);
                        if (q != m_numberStates - 1){
                            formula += " | ";
                        }
                    }
                    formula += " ) &\n";
                }

                formula = formula.substr(0, formula.length() - 3);
                stream << formula;

                //Equation 4.3.2
                formula = " &\n";
                for (const auto &u : m_prefixes) {
                    formula += " ( ";
                    for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                        for (unsigned int p = 0 ; p < m_numberStates ; p++) {
                            if (p != q) {
                                formula += " ! ( x_" + u + "_" + std::to_string(q) + " & x_" + u + "_" + std::to_string(p) + " )";
                                formula += " &";
                            }
                        }
                    }
                    formula = formula.substr(0, formula.length() - 2);
                    formula += " ) &\n";
                }

                formula = formula.substr(0, formula.length() - 3);
                stream << formula;

                //Equation 4.3.3
                formula = " &\n";
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
                                    formula += "( ( x_" + u + "_" + std::to_string(p) + " & " +
                                                    "x_" + v + "_" + std::to_string(p) + " ) " + " -> ( "
                                                    "x_" + ua + "_" + std::to_string(q) + " <-> " +
                                                    "x_" + va + "_" + std::to_string(q) + " ) )";
                                    formula += " &\n";
                                }
                            }
                        }
                    }
                }

                formula = formula.substr(0, formula.length() - 3);
                stream << formula;

                //Equation 4.3.4
                formula = " &\n";
                for (const auto &u : m_Sp) {
                    for (const auto &v : m_Sm) {
                        for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                            formula += " ! ( x_" + u + "_" + std::to_string(q) + " & x_" + v + "_" + std::to_string(q) + " )";
                            formula += " &\n";
                        }
                    }
                }
                formula = formula.substr(0, formula.length() - 3);
                stream << formula;

                return true;
            }

            std::unique_ptr<DFA<char>> UnaryNonCNFMethod::toDFA(std::istream &stream) const {
                //First, we have to read the solution
                std::string line;
                std::map<std::string, unsigned int> words_states;

                while(std::getline(stream, line)){
                    //If there is a variable on this line
                    if (line.at(0) == 'x'){
                        std::vector<std::string> results;
                        std::vector<std::string> results_2;
                        //We split the line to get var - = - assignationOfTheVar
                        boost::split(results, line, [](char c){return c == ' ';});
                        //We split the var to get x - pref - stateNumber
                        boost::split(results_2, results[0], [](char c){return c == '_';});
                        // auto pair = std::make_pair(results_2[1], std::stoi(results_2[2]));
                        // m_state_bit.emplace(state(pair, std::stoi(results[2])));
                        if (std::stoi(results[2]) == 1){
                            words_states.emplace(results_2[1], std::stoi(results_2[2]));
                        }
                    }
                }

                std::unique_ptr<DFA<char>> dfa = std::make_unique<DFA<char>>(words_states.find("") -> second, m_Sp.find("") != m_Sp.end());

                for(const auto &u : m_prefixes){
                    unsigned int state = words_states.find(u) -> second;
                    //If the state is final
                    if(std::find(m_Sp.begin(), m_Sp.end(), u) != m_Sp.end()){
                        dfa->addState(state, true);
                    }
                    else{
                        dfa->addState(state, false);
                    }
                }

                for (unsigned int i = 0 ; i < m_numberStates ; i++) {
                    for (const char &a : m_alphabet) {
                        bool found = false;
                        for (unsigned int j = 0 ; j < m_numberStates ; j++) {
                            for (const auto &ua : m_prefixes) {
                                if (ua.size() > 0 && ua.at(ua.size() - 1) == a){
                                    std::string u = ua.substr(0, ua.size() - 1);

                                    if (words_states.at(u) == i && words_states.at(ua) == j) {
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

            std::string UnaryNonCNFMethod::getFileName() const {
                return "unary";
            }
        }
    }
}