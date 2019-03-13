#pragma once

#include <set>
#include <string>

namespace passive {
    /**
     * Computes \f$S = S_+ \cup S_-\f$.
     * 
     * In theory, we have \f$S = (S_+, S_-)\f$. However, we need to compute \f$Pref(S_+, S_-)\f$. Therefore, it's easier to have \f$S = S_+ \cup S_-\f$ and compute \f$Pref(S)\f$.
     * @param Sp The \f$S_+\f$ set
     * @param Sm The \f$S_-\f$ set
     * @return The set \f$S = S_+ \cup S_-\f$
     */
    std::set<std::string> computeS(const std::set<std::string> &Sp, const std::set<std::string> &Sm);

    /**
     * Computes \f$Pref(S)\f$.
     * @param S The set \f$S\f$
     */
    std::set<std::string> computePrefixes(const std::set<std::string> &S);

    /**
     * Computes the alphabet \f$\Sigma\f$ from the words in \f$S\f$.
     * @param S The set \f$S\f$
     */
    std::set<char> computeAlphabet(const std::set<std::string> &S);

    /**
     * Reads the \f$S_+\f$ and \f$S_-\f$ sets from a file
     * @param inputFile The path to the file to read
     * @param Sp The set \f$S_+\f$ to fill
     * @param Sm The set \f$S_-\f$ to fill
     */
    void readFromFile(const std::string &inputFile, std::set<std::string> &Sp, std::set<std::string> &Sm);

    /**
     * Generates randomly a sample set. The alphabet's size is at most 10.
     * 
     * The functions ensures that Sp and Sm do not contain a common word.
     * It must be possible to generate enough words. Otherwise, this function will never terminates.
     * @param numberWords The total number of words to generate
     * @param minLength The minimum size of each word
     * @param maxLength The maximum size of each word
     * @param alphabetSize The size of the alphabet. Since symbols are taken randomly in the alphabet, it may happen that a sampling set uses a smaller alphabet.
     * @param Sp The words to accept are put in this set
     * @param Sm The words to reject are put in this set
     * @param SpProbability The probability that a word is put in Sp (by default, 0.5)
     */
    void generateRandomly(unsigned int numberWords, unsigned int minLength, unsigned int maxLength, unsigned int alphabetSize, std::set<std::string> &Sp, std::set<std::string> &Sm, double SpProbability = 0.5);
}