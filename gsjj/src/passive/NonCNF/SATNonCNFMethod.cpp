#include "passive/NonCNF/SATNonCNFMethod.h"

#include <fstream>
#include <iostream>
#include <functional>

#include "utils/System.h"
using namespace Minisat;

namespace passive {
    namespace NonCNF {
        SATNonCNFMethod::SATNonCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n, const bool *stopTrigger) :
            Method(SpSet, SmSet, SSet, prefixesSet, alphabetSet, n),
            mustStop(stopTrigger) {
        }

        SATNonCNFMethod::~SATNonCNFMethod() {

        }

        bool SATNonCNFMethod::solve() {
            const std::string& in = getInputFile();
            const std::string& out = getOutputFile();
            std::ofstream output(in);
            if (createFormula(output)) {
                output.close();
                m_triedSolve = true;
                const char* argv[] = {
                    "./limboole",
                    "-s",
                    in.c_str(),
                    "-o",
                    out.c_str(),
                };

                if (*mustStop) {
                    return false;
                }

                m_cpuTimeStart = cpuTime();
                limboole(5, argv, mustStop);
                m_cpuTimeEnd = cpuTime();

                return m_hasSolution = isSatisfiable();
            }
            else {
                output.close();
                // TODO: is it the same condition for every method?
                return m_hasSolution = (m_Sp.size() == 0 || m_Sm.size() == 0);
            }
        }

        bool SATNonCNFMethod::hasSolution() const {
            return m_hasSolution;
        }

        std::unique_ptr<DFA<char>> SATNonCNFMethod::constructDFA() {
            if (!m_triedSolve) {
                if (!solve()) {
                    return nullptr;
                }
            }

            if (hasSolution()) {
                if (m_Sp.size() == 0 || m_Sm.size() == 0) {
                    std::unique_ptr<DFA<char>> dfa = std::make_unique<DFA<char>>(0, m_Sp.size() != 0);
                    for (const char &a : m_alphabet) {
                        dfa->addTransition(0, a, 0);
                    }
                    return dfa;
                }
                std::ifstream file(getOutputFile());
                auto dfa = toDFA(file);
                file.close();
                return dfa;
            }
            return nullptr;
        }

        void SATNonCNFMethod::printVariables() const {
            std::cout << "NOT IMPLEMENTED\n";
        }

        bool SATNonCNFMethod::isSatisfiable() const {
            std::ifstream file(getOutputFile());
            std::string line;
            std::getline(file, line);
            if (line.find("UNSATISFIABLE") != std::string::npos) {
                return false;
            }
            else {
                return true;
            }
        }

        std::string SATNonCNFMethod::getInputFile() const {
            // TODO: find a way for Windows
            return "/tmp/" + getFileName() + std::to_string(m_numberStates);
        }

        std::string SATNonCNFMethod::getOutputFile() const {
            return getInputFile() + "-test";
        }
    }
}