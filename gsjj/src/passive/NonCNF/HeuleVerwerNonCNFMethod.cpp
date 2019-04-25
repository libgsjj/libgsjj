/**
 * \file HeuleVerwerNonCNFMethod.cpp
 * 
 * This file implements the Heule and Verwer method that relies on non CNF SAT solver.
 */

#include "gsjj/passive/NonCNF/HeuleVerwerNonCNFMethod.h"

#include <iostream>
#include <utility>
#include <cmath>
#include <vector>
#include <fstream>
#include <math.h>
#include <boost/algorithm/string.hpp>

namespace gsjj {
    namespace passive {
        namespace NonCNF {
            HeuleVerwerNonCNFMethod::HeuleVerwerNonCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n) :
                SATNonCNFMethod(SpSet, SmSet, SSet, prefixesSet, alphabetSet, n)
            {
                s_registered = s_registered;
            }

            HeuleVerwerNonCNFMethod::~HeuleVerwerNonCNFMethod()
            {
            }

            std::string HeuleVerwerNonCNFMethod::getFactoryName() {
                return "heuleNonCNF";
            }

            bool HeuleVerwerNonCNFMethod::createFormula(std::ostream &stream){
                std::string formula = "";
                //Equation 4.5.1
                if (m_numberStates > 1) {
                    for (unsigned int p = 0; p < m_numberStates; p++){
                        formula += " ( ";
                        for (const char &a : m_alphabet){
                            formula += " ( ";
                            for (unsigned int q = 0; q < m_numberStates; q++){
                                for (unsigned int r = 0; r < m_numberStates; r++){
                                    //r is q' in the report
                                    if (q != r){
                                        formula += "! ( d_" + std::to_string(p) + "_" + a + "_" + std::to_string(q) + 
                                                    " & d_" + std::to_string(p) + "_" + a + "_" + std::to_string(r) + " )";
                                        formula += " &\n";
                                    }
                                }
                            }
                            formula = formula.substr(0, formula.length() - 3);
                            formula += " ) &\n";
                        }
                        formula = formula.substr(0, formula.length() - 3);
                        formula += " ) &\n";
                    }

                    formula = formula.substr(0, formula.length() - 3);
                    stream << formula;

                    formula = " &\n";
                }

                //Equation 4.5.2
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

                //Equation 4.5.3
                formula = " &\n";
                for (const auto &ua : m_prefixes) {
                    if (ua.size() > 0) {
                        const std::string &u = ua.substr(0, ua.size() - 1);
                        const char &a = ua.at(ua.size() - 1);

                        for (unsigned int p = 0 ; p < m_numberStates ; p++) {
                            for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                                formula += "( ( x_" + u + "_" + std::to_string(p) + 
                                            " & d_" + std::to_string(p) + "_" + a + "_" + std::to_string(q) + " ) -> x_" +
                                            ua + "_" + std::to_string(q) + " )";
                                formula += " &\n";
                            }
                        }
                    }
                }

                formula = formula.substr(0, formula.length() - 3);
                stream << formula;

                //Equation 4.5.4
                formula = " &\n";
                for (unsigned int q = 0; q < m_numberStates; q++){
                    for (const auto &u : m_Sp){
                        formula += "( x_" + u + "_" + std::to_string(q) + " -> f_" + std::to_string(q) + " )";
                        formula += " &\n";
                    }

                    for (const auto &u : m_Sm){
                        formula += "( x_" + u + "_" + std::to_string(q) + " -> ! f_" + std::to_string(q) + " )";
                        formula += " &\n";
                    }
                }

                formula = formula.substr(0, formula.length() - 3);
                stream << formula;

                //Equation 4.5.5
                formula = " &\n";
                for (unsigned int p = 0; p < m_numberStates; p++){
                    for (const char &a : m_alphabet){
                        formula += "( ";
                        for (unsigned int q = 0; q < m_numberStates; q++){
                            formula += "d_" + std::to_string(p) + "_" + a + "_" + std::to_string(q);
                            formula += " | ";
                        }
                        formula = formula.substr(0, formula.length() - 3);
                        formula += ") &\n";
                    }
                }

                formula = formula.substr(0, formula.length() - 3);
                stream << formula;

                //Equation 4.5.6
                formula = " &\n";
                for (const auto &u : m_prefixes) {
                    for (unsigned int p = 0 ; p < m_numberStates ; p++) {
                        for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                            if (p != q) {
                                formula += "( ! ( x_" + u + "_" + std::to_string(p) + " & x_" +
                                            u + "_" + std::to_string(q) + " ) )";
                                formula += " &\n";
                            }
                        }
                    }
                }

                formula = formula.substr(0, formula.length() - 3);
                stream << formula;

                //Equation 4.5.7
                formula = " &\n";
                for (const auto &ua : m_prefixes) {
                    if (ua.size() > 0) {
                        const std::string &u = ua.substr(0, ua.size() - 1);
                        const char &a = ua.at(ua.size() - 1);

                        for (unsigned int p = 0 ; p < m_numberStates ; p++) {
                            for (unsigned int q = 0 ; q < m_numberStates ; q++) {
                                formula += "( ( x_" + u + "_" + std::to_string(p) + " & " + "x_" + ua + "_" + std::to_string(q) + " ) "
                                            + "-> d_" + std::to_string(p) + "_" + a + "_" + std::to_string(q) + " ) ";
                                formula += "&\n";
                            }
                        }
                    }
                }

                formula = formula.substr(0, formula.length() - 3);
                stream << formula;

                return true;
            }

            std::unique_ptr<DFA<char>> HeuleVerwerNonCNFMethod::toDFA(std::istream &stream) const
            {
                //First, we have to read the solution
                std::string line;
                // std::map<std::string, unsigned int> words_states;
                // std::vector<unsigned int> final_states;
                std::vector<std::vector<std::string>> d_vars;
                std::vector<unsigned int> f_vars;

                std::unique_ptr<DFA<char>> dfa;

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
                        if (std::stoi(results[2]) == 1 && results_2[1] == ""){
                            dfa = std::make_unique<DFA<char>>(std::stoi(results_2[2]), m_Sp.find("") != m_Sp.end());
                        }
                    }
                    if (line.at(0) == 'd'){
                        std::vector<std::string> results;
                        std::vector<std::string> results_2;
                        //We split the line to get var - = - assignationOfTheVar
                        boost::split(results, line, [](char c){return c == ' ';});
                        //We split the var to get d - p - a - q
                        boost::split(results_2, results[0], [](char c){return c == '_';});
                        // auto pair = std::make_pair(results_2[1], std::stoi(results_2[2]));
                        // m_state_bit.emplace(state(pair, std::stoi(results[2])));
                        if(std::stoi(results[2]) == 1){
                            std::vector<std::string> my_vec;
                            my_vec.push_back(results_2[1]);
                            my_vec.push_back(results_2[2]);
                            my_vec.push_back(results_2[3]);
                            d_vars.push_back(my_vec);
                        }
                    }
                    if (line.at(0) == 'f'){
                        std::vector<std::string> results;
                        std::vector<std::string> results_2;
                        //We split the line to get var - = - assignationOfTheVar
                        boost::split(results, line, [](char c){return c == ' ';});
                        //We split the var to get f - q
                        boost::split(results_2, results[0], [](char c){return c == '_';});
                        // auto pair = std::make_pair(results_2[1], std::stoi(results_2[2]));
                        // m_state_bit.emplace(state(pair, std::stoi(results[2])));
                        if (std::stoi(results[2]) == 1){
                            f_vars.push_back(std::stoi(results_2[1]));
                        }
                    }
                }

                for(unsigned int i = 0; i < m_numberStates; i++){
                    if(std::find(f_vars.begin(), f_vars.end(), i) != f_vars.end()){
                        dfa->addState(i, true);
                    }
                    else{
                        dfa->addState(i, false);
                    }
                }

                for(const auto &vec : d_vars){
                    const char *a = vec.at(1).c_str();
                    dfa->addTransition(std::stoi(vec.at(0)), a[0], std::stoi(vec.at(2)));
                }

                return dfa;
            }

            std::string HeuleVerwerNonCNFMethod::getFileName() const {
                return "heule";
            }
        }
    }
}