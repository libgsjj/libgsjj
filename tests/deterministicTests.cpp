#include "catch.hpp"

#include <set>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>

#include "DFA.h"
#include "passive/Method.h"
#include "passive/utils.h"

#include "utils.h"

/**
 * Requires that the methods construct optimal DFA of the expected size
 * @param Sp The \f$S_+\f$
 * @param Sm The \f$S_-\f$
 * @param expectedSize The expected size of the DFA
 */
void performTests(passive::Methods met, const std::set<std::string> &Sp, const std::set<std::string> &Sm, unsigned int expectedSize) {
    std::unique_ptr<passive::Method> method;
    bool success;
    std::tie(method, success) = passive::Method::constructMethod(met, Sp, Sm);

    REQUIRE(method);
    REQUIRE(success);

    REQUIRE(method->solve());
    REQUIRE(method->hasSolution());
    REQUIRE(method->numberOfStates() == expectedSize);

    std::shared_ptr<DFA<char>> dfa = std::move(method->constructDFA());
    REQUIRE(dfa);

    checkConsistency(dfa, Sp, Sm);

    REQUIRE(method->numberOfStates() == dfa->getNumberOfStates());
}

void testMethod(passive::Methods met) {
    std::set<std::string> Sp, Sm;
    SECTION("Sp = {aab, aba, ba, babb} and Sm = {epsilon, bb}") {
        Sp = {"aab", "aba", "ba", "babb"};
        Sm = {"", "bb"};

        performTests(met, Sp, Sm, 2);
    }

    SECTION("Sp = {aa, ba, aba} and Sm = {epsilon, ab}") {
        Sp = {"aa", "ba", "aba"};
        Sm = {"", "ab"};

        performTests(met, Sp, Sm, 2);
    }

    SECTION("Sp = {epsilon, 001, 1, 10, 100, 111} and Sm = {0, 01, 00, 000}") {
        Sp = {"", "001", "1", "10", "100", "111"};
        Sm = {"0", "01", "00", "000"};

        performTests(met, Sp, Sm, 4);
    }

    SECTION("Sp = {1, 0, 00, 10} and Sm = {epsilon, 11}") {
        Sp = {"1", "0", "00", "10"};
        Sm = {"", "11"};

        performTests(met, Sp, Sm, 2);
    }
}

TEST_CASE("The Biermann and Feldman method builds an optimal DFA", "[passive][optimal][biermann]") {
    testMethod(passive::Methods::BIERMANN);
}

TEST_CASE("The unary method builds an optimal DFA", "[passive][optimal][unary]") {
    testMethod(passive::Methods::UNARY);
}

TEST_CASE("The binary method builds an optimal DFA", "[passive][optimal][binary]") {
    testMethod(passive::Methods::BINARY);
}

TEST_CASE("The Heule and Verwer method builds an optimal DFA", "[passive][optimal][heule]") {
    testMethod(passive::Methods::HEULEVERWER);
}

TEST_CASE("The Neider and Jansen method builds an optimal DFA", "[passive][optimal][neider]") {
    testMethod(passive::Methods::NEIDERJANSEN);
}

TEST_CASE("The unaryNonCNF method builds an optimal DFA", "[passive][optimal][unaryNonCNF]") {
    testMethod(passive::Methods::UNARY_NON_CNF);
}

TEST_CASE("The binaryNonCNF method builds an optimal DFA", "[passive][optimal][binaryNonCNF]") {
    testMethod(passive::Methods::BINARY_NON_CNF);
}

TEST_CASE("The Heule and Verwer nonCNF method builds an optimal DFA", "[passive][optimal][heuleNonCNF]") {
    testMethod(passive::Methods::HEULE_NON_CNF);
}