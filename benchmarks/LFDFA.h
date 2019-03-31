#pragma once

#include <set>

#include "gsjj/DFA.h"

/**
 * Loop-free DFA.
 * 
 * This kind of DFA does not contain any loops. Its main purpose is to load a LFDFA from a file and produce the sets \f$S_+\f$ and \f$S_-\f$.
 * 
 * It's also an implementation of our definition of a prefix-acceptor DFA.
 */
class LFDFA : public gsjj::DFA<char> {
public:
    /**
     * Loads the LFDFA from a file. For the file structure, see the README file
     * @param filepath The path to the file to load
     * @return The LFDFA built from the file
     */
    static std::unique_ptr<LFDFA> loadFromFile(const std::string &filepath);

public:
    LFDFA();

    /**
     * Fills the sets Sp and Sm from the LFDFA.
     * @param Sp The set \f$S_+\f$ to fill
     * @param Sm The set \f$S_-\f$ to fill
     */
    void getSets(std::set<std::string> &Sp, std::set<std::string> &Sm);

private:
    /**
     * Recursive implementation for getSets. It's basically a DFS
     * @param Sp The set \f$S_+\f$ to fill
     * @param Sm The set \f$S_-\f$ to fill
     * @param state The current state
     * @param string The string built so far
     */
    void getSets(std::set<std::string> &Sp, std::set<std::string> &Sm, unsigned int state, std::string &string);
};