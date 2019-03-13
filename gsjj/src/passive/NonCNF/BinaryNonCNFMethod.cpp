#include "passive/NonCNF/BinaryNonCNFMethod.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <math.h>
#include <boost/algorithm/string.hpp>

#include "utils/System.h"
using namespace Minisat;

extern "C" {
#include "limboole.h"
}

#include "binary.h"

namespace passive {
    namespace NonCNF {
        BinaryNonCNFMethod::BinaryNonCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n, const bool *stopTrigger) :
            SATNonCNFMethod(SpSet, SmSet, SSet, prefixesSet, alphabetSet, n, stopTrigger) {
        }

        BinaryNonCNFMethod::~BinaryNonCNFMethod() {
        }

        std::string BinaryNonCNFMethod::phi_different(std::string pref1, std::string pref2){
            unsigned int m = ceil(log2(m_numberStates));
            std::string toWrite = "(";
            for (unsigned int i = 0; i < m; i++){
                toWrite = toWrite +
                    "(" +
                        "(" +
                            "x_" + pref1 + "_" + std::to_string(i) +
                            " & !" +
                            "x_" + pref2 + "_" + std::to_string(i) +
                        ")" +
                    "|" +
                        "(" +
                            "!x_" + pref1 + "_" + std::to_string(i) +
                            " & " +
                            "x_" + pref2 + "_" + std::to_string(i) +
                        ")" +
                    ")";
                if(i != m - 1){
                    toWrite += " | ";
                }
            }
            return toWrite + ")";
        }

        std::string BinaryNonCNFMethod::phi_not_q_i(std::string pref, unsigned int i){
            unsigned int m = ceil(log2(m_numberStates));
            std::vector<bool> bin_i = decimal_to_binary(i, m);
            std::string toWrite = "(";
            for (unsigned int j = 0; j < m; j++){
                if(bin_i.at(j) == 0){
                    toWrite = toWrite + "x_" + pref + "_" + std::to_string(j);
                }
                else{
                    toWrite = toWrite + "!" + "x_" + pref + "_" + std::to_string(j);
                }
                if(j != m - 1){
                    toWrite += " | ";
                }
            }
            return toWrite + ")";
        }

        bool BinaryNonCNFMethod::createFormula(std::ostream &stream) {
            unsigned int m = ceil(log2(m_numberStates));

            if (m == 0) {
                return false;
            }

            //Traduction of the first equation of Biermann and Feldman
            std::string formula = "";
            for (const auto &pref : m_prefixes){
                if (pref.size() > 0) {
                    const std::string &u = pref.substr(0, pref.length() - 1);
                    const char &a = pref.at(pref.length() - 1);

                    for (const auto &pref2 : m_prefixes){
                        if(pref2.size() > 0 && a == pref2.at(pref2.length() - 1)){
                            const std::string &v = pref2.substr(0, pref2.length() - 1);
                            //Phi different for u and u'
                            std::string phi_dif_u = phi_different(u, v);
                            //Phi different for ua and u'a
                            std::string phi_dif_ua = phi_different(pref, pref2);
                            //Aggregation of the two formulas to respect the Biermann and Feldman formula
                            formula = formula + "(" + phi_dif_u + " | !" + phi_dif_ua + ")\n";
                            formula += " &\n";
                        }
                    }
                }
            }
            formula = formula.substr(0, formula.length() - 3) + "\n\n";
            stream << formula;

            //Traduction of the second equation of Biermann and Feldman
            formula = " &\n";
            for (const auto &pos : m_Sp){
                for (const auto &neg : m_Sm){
                    formula += phi_different(pos, neg) + " &";
                    formula += "\n";
                }
            }
            formula = formula.substr(0, formula.length() - 3) + "\n\n";
            stream << formula;

            formula = " &\n";
            for (const auto &pref : m_prefixes){
                for (unsigned int i = m_numberStates; i < pow(2, m); i++){
                    std::string phi_q = phi_not_q_i(pref, i);
                    formula += "(" + phi_q + ")";
                    formula += " &\n";
                }
            }
            formula = formula.substr(0, formula.length() - 3);
            stream << formula;

            return true;
        }

        std::unique_ptr<DFA<char>> BinaryNonCNFMethod::toDFA(std::istream &stream) const {
            //1 : Building of the arrays for each prefix

            std::string line;
            unsigned int m = ceil(log2(m_numberStates));
            std::map<std::pair<std::string, unsigned int>, unsigned int> m_state_bit;

            while(std::getline(stream, line)){
                //If there is a variable on this line
                if (line.at(0) == 'x'){
                    std::vector<std::string> results;
                    std::vector<std::string> results_2;
                    //We split the line to get var - = - assignationOfTheVar
                    boost::split(results, line, [](char c){return c == ' ';});
                    //We split the var to get x - pref - posOfTheBit
                    boost::split(results_2, results[0], [](char c){return c == '_';});
                    auto pair = std::make_pair(results_2[1], std::stoi(results_2[2]));
                    m_state_bit.emplace(state(pair, std::stoi(results[2])));
                }
            }

            //A map to remain the state associated to a word from S
            std::map<std::string, unsigned int> words_states;

            //Adding the words from Sp
            // for(const auto &u : m_Sp){
            //     std::vector<unsigned int> binary_number;
            //     for (unsigned int i = 0; i < m; i++){
            //         binary_number.push_back(m_state_bit.find(std::make_pair(u, i))->second);
            //     }
            //     unsigned int bin = binary_to_decimal(binary_number);
            //     words_states.emplace(u, bin);
            // }

            //Adding the words from S
            for(const auto &u : m_prefixes){
                std::vector<bool> binary_number;
                for (unsigned int i = 0; i < m; i++){
                    binary_number.push_back(m_state_bit.at(std::make_pair(u, i)));
                }
                unsigned int bin = binary_to_decimal(binary_number);
                words_states.emplace(u, bin);
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

        std::string BinaryNonCNFMethod::getFileName() const {
            return "binary";
        }
    }
}