#include "passive/utils.h"

#include <algorithm>
#include <fstream>
#include <random>

namespace gsjj {
    namespace passive {
        std::set<std::string> computeS(const std::set<std::string> &Sp, const std::set<std::string> &Sm) {
            std::set<std::string> S;
            std::set_union(Sp.begin(), Sp.end(), Sm.begin(), Sm.end(), std::inserter(S, S.begin()));
            return S;
        }

        std::set<std::string> computePrefixes(const std::set<std::string> &S) {
            std::set<std::string> prefixes;
            for (const auto &s : S) {
                for (std::size_t i = 0; i <= s.size(); i++) {
                    prefixes.insert(s.substr(0, i));
                }
            }
            return prefixes;
        }

        std::set<char> computeAlphabet(const std::set<std::string> &S) {
            std::set<char> alphabet;
            for (const auto &s : S) {
                for (const auto &c : s) {
                    alphabet.insert(c);
                }
            }
            return alphabet;
        }

        void readFromFile(const std::string &inputFile, std::set<std::string> &Sp, std::set<std::string> &Sm) {
            std::ifstream in(inputFile);
            std::string s;
            bool accept = true;
            while (getline(in, s)) {
                if (s == "=====") {
                    accept = false;
                }
                else if (accept) {
                    Sp.insert(s);
                }
                else {
                    Sm.insert(s);
                }
            }
        }

        void generateRandomly(unsigned int numberWords, unsigned int minLength, unsigned int maxLength, unsigned int alphabetSize, std::set<std::string> &Sp, std::set<std::string> &Sm, double SpProbability) {
            std::default_random_engine generator(time(0));
            std::bernoulli_distribution toAcceptDistribution(SpProbability);
            std::uniform_int_distribution<char> symbolDistribution('a', 'a' + alphabetSize - 1);
            std::uniform_int_distribution<unsigned int> lengthDistribution(minLength, maxLength);

            while (Sp.size() + Sm.size() < numberWords) {
                std::string word;

                do {
                    unsigned int length = lengthDistribution(generator);
                    word.clear();
                    word.reserve(length);
                    for (std::size_t j = 0 ; j < length ; j++) {
                        word.push_back(symbolDistribution(generator));
                    }
                } while (Sp.find(word) != Sp.end() || Sm.find(word) != Sm.end());

                if (toAcceptDistribution(generator)) {
                    Sp.insert(word);
                }
                else {
                    Sm.insert(word);
                }
            }
        }
    }
}