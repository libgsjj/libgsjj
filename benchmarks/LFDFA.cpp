#include "LFDFA.h"

#include <fstream>
#include <iostream>

std::unique_ptr<LFDFA> LFDFA::loadFromFile(const std::string &filepath) {
    std::ifstream file(filepath);
    if (!file) {
        throw std::invalid_argument(filepath + " could not be open");
    }
    std::unique_ptr<LFDFA> dfa = std::make_unique<LFDFA>();
    std::string line;

    while(std::getline(file, line)) {
        // We skip the lines starting by a dot
        if (line.at(0) == '.') {
            continue;
        }

        char symbol;
        std::string startState, endState;
        bool accepting;
        std::stringstream ss(line);
        if (!(ss >> symbol >> startState >> endState >> accepting)) {
            throw std::runtime_error(filepath + " contains an invalid line: " + line);
        }

        unsigned int startStateNumber = 0;
        ss.clear();
        ss.str(startState.substr(1));
        if (!(ss >> startStateNumber)) {
            throw std::runtime_error(filepath + " contains an invalid line: " + line + ". " + startState + " is an invalid state name");
        }

        unsigned int endStateNumber = 0;
        ss.clear();
        ss.str(endState.substr(1));
        if (!(ss >> endStateNumber)) {
            throw std::runtime_error(filepath + " contains an invalid line: " + line + ". " + endState + " is an invalid state name");
        }

        if (!dfa->isState(endStateNumber)) {
            dfa->addState(endStateNumber, accepting);
        }

        dfa->addTransition(startStateNumber, symbol, endStateNumber);
    }

    return dfa;
}

LFDFA::LFDFA() :
    DFA(0, false) {

}

void LFDFA::getSets(std::set<std::string> &Sp, std::set<std::string> &Sm) {
    std::string string = "";
    getSets(Sp, Sm, 0, string);
}

void LFDFA::getSets(std::set<std::string> &Sp, std::set<std::string> &Sm, unsigned int state, std::string &string) {
    if (isAcceptingState(state)) {
        Sp.insert(string);
    }
    else {
        Sm.insert(string);
    }

    auto &delta = getTransitions();

    auto transitionsFromState = delta.find(state);

    // state does not have any outgoing transition
    if (transitionsFromState == delta.cend()) {
        return;
    }

    for (auto transition : transitionsFromState->second) {
        string += transition.first;
        getSets(Sp, Sm, transition.second, string);
        string.pop_back();
    }
}