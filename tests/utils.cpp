#include "utils.h"

#include <iostream>

#include "catch.hpp"

void checkConsistency(std::shared_ptr<DFA<char>> dfa, const std::set<std::string> &Sp, const std::set<std::string> &Sm) {
    SECTION("The DFA is consistent") {
        for (const auto &word : Sp) {
            REQUIRE(dfa->isAccepted(std::list<char>(word.begin(), word.end())));
        }

        for (const auto &word : Sm) {
            REQUIRE_FALSE(dfa->isAccepted(std::list<char>(word.begin(), word.end())));
        }
    }
}