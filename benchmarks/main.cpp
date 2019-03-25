#include <iostream>
#include <istream>
#include <array>

#include <boost/program_options.hpp>
#include <cvc4/cvc4.h>

#include "gsjj/passive/Method.h"
#include "gsjj/passive/utils.h"
#include "gsjj/passive/MethodFactory.h"

namespace po = boost::program_options;
using namespace gsjj;

/**
 * Prints Sp and Sm. Mainly for debug purposes
 * @param Sp The Sp set
 * @param Sm The Sm set
 */
void print_Sp_Sm(const std::set<std::string> &Sp, const std::set<std::string> &Sm) {
    std::cout << "Sp = {";
    bool first = true;
    for (const auto &s : Sp) {
        if (!first) {
            std::cout << ", ";
        }
        std::cout << s;
        first = false;
    }
    std::cout << "}, " << Sp.size() << "\n";

    std::cout << "Sm = {";
    first = true;
    for (const auto &s : Sm) {
        if (!first) {
            std::cout << ", ";
        }
        std::cout << s;
        first = false;
    }
    std::cout << "}, " << Sm.size() << "\n";
}

/**
 * Constructs a method and solves it for a fixed n
 * @param Sp The Sp set
 * @param Sm The Sm set
 * @param S The S set
 * @param prefixes The prefixes set
 * @param alphabet The alphabet set
 * @param n The number of states
 * @param choice The choice of method to construct
 * @return The method already solved
 */
std::unique_ptr<passive::Method> call_method_fixed_n(const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::set<std::string> &S, const std::set<std::string> &prefixes, const std::set<char> &alphabet, unsigned int n, const std::string &choice) {
    std::unique_ptr<passive::Method> method = passive::constructMethod(choice, n, Sp, Sm, S, prefixes, alphabet);

    method->solve();
    std::cout << "TIME TAKEN: " << method->timeToSolve() << "\n";

    if (!method->hasSolution()) {
        std::cout << "Impossible to construct a DFA with only " << n << " states\n";
        return nullptr;
    }
    else {
        std::cout << "It's possible to construct a DFA with " << n << " states\n";
    }
    
    return std::move(method);
}

std::tuple<long double, long double> values(std::vector<long double> &times) {
    long double mean = 0;
    for (auto &a : times) {
        mean += a;
    }
    mean /= times.size();

    long double median;
    std::sort(times.begin(), times.end());
    if (times.size() % 2 == 0) {
        median = (times[times.size() / 2 - 1] + times[times.size() / 2]) / 2;
    }
    else {
        median = times[times.size() / 2];
    }

    return std::make_tuple(mean, median);
}

/**
 * Tests every and each method on random samples. Each method works on the same samples.
 * 
 * The output is produced in the file "full.time". The first column is the size of the samples set. The second column is the mean execution time for Biermann and the third is the median execution time for Biermann. The fourth and fifth are for the unary method. The sixth and seventh are for Heule and Verwer while the eighth and nineth are for Neider and Jansen.
 * 
 * @param minSize The minimal size of the samples set
 * @param maxSize The maximal size of the samples set
 * @param nGenerations The number of samples set generated for a given size
 * @param minWordSize The minimal size of each generated word
 * @param maxWordSize The maximal size of each generated word
 * @param alphabetSize The size of the alphabet. Since symbols are taken randomly in the alphabet, it may happen that a sampling set uses a smaller alphabet.
 * @param SpProbability The probability that a word ends in Sp
 * @param timeLimit The time limit in seconds
 * @param verbose Whether to print information during the benchmark run
 */
void random_benchmark(unsigned int minSize, unsigned int maxSize, unsigned int nGenerations, unsigned int minWordSize, unsigned int maxWordSize, unsigned int alphabetSize, double SpProbability, unsigned int timeLimit, bool verbose) {
    std::ofstream output("full-" + std::to_string(minSize) + "-" + std::to_string(maxSize) + "-" + std::to_string(nGenerations) + "-" + std::to_string(minWordSize) + "-" + std::to_string(maxWordSize) + "-" + std::to_string(alphabetSize) + ".time");
    output << "# Size; ";

    // First, we create an array to store the execution times
    // We need one array by method
    std::array<std::vector<long double>, passive::allMethods.size()> times;
    // We also need to count the number of timeouts
    std::array<unsigned int, passive::allMethods.size()> timeouts;
    for (unsigned int c = 0 ; c < passive::allMethods.size() ; c++) {
        times[c].resize(nGenerations);
        output << passive::allMethods[c] << " mean median timeouts;" << " ";
    }
    output << "\n";

    // Now, we create nGenerations samples (the size varies from minSize to maxSize) and we execute every method on these samples
    for (unsigned int n = minSize ; n <= maxSize ; n++) {
        timeouts.fill(0);

        for (unsigned int generation = 0 ; generation < nGenerations ; generation++) {
            std::set<std::string> Sp, Sm;
            passive::generateRandomly(n, minWordSize, maxWordSize, alphabetSize, Sp, Sm, SpProbability);
            std::set<std::string> S = passive::computeS(Sp, Sm);
            std::set<char> alphabet = passive::computeAlphabet(S);
            std::set<std::string> prefixes = passive::computePrefixes(S);

            if (verbose) {
                std::cout << "Size: " << n << "; generation: " << generation+1 << "/" << nGenerations << "\n";
            }

            // This should be a case of an embarrassingly parallel problem. Therefore, we only have to activate the parallelisation (using OpenMP)
            #pragma omp parallel for schedule(dynamic)
            for (unsigned int c = 0 ; c < passive::allMethods.size() ; c++) {
                long double timeTaken = 0;
                std::unique_ptr<passive::Method> ptr;
                bool success;
                std::tie(ptr, success) = passive::constructMethod(passive::allMethods[c], Sp, Sm, S, prefixes, alphabet, std::chrono::seconds(timeLimit), &timeTaken);
                
                times[c][generation] = timeTaken;
                if (!success) {
                    timeouts[c] += 1;
                }
                if (verbose) {
                    std::cout << passive::allMethods[c] << " ";
                }
            }
            if (verbose) {
                std::cout << "\n";
            }
        }

        output << n << " ";
        for (unsigned int c = 0 ; c < passive::allMethods.size() ; c++) {
            long double mean, median;
            std::tie(mean, median) = values(times[c]);
            output << mean << " " << median << " " << timeouts[c] << " ";
        }
        output << "\n";
        output.flush();
    }

    output.close();
}

int main(int argc, char** argv) {
    std::string choice;
    std::string inputFile, outputFile;
    bool toDot, rand_bench, verbose;
    unsigned int n, numberWords, minSize, maxSize, nGenerations, wordSize, minWordSize, maxWordSize, alphabetSize, timeLimit;
    double probabilityAccepted;

    po::options_description desc("Benchmarks for learning DFA algorithms. For the moment, it only works for passive algorithms.");
    desc.add_options()
        ("help,h", "produce help message")
        ("verbose,v", po::bool_switch(&verbose), "If set, the program outputs more information")

        ("method", po::value<std::string>(&choice), "The method to use [MANDATORY if random-benchmarks is not set]. The '--method' part is not necessary (positional argument)")
        ("number-states,n", po::value<unsigned int>(&n)->notifier([](unsigned int i) {
                if (i < 1) {
                    throw std::runtime_error("--number-states (or -n) must be greater or equal to 1");
                }
            }), "If set, the program tries to construct a DFA with exactly n states. If not set, the minimal n is seeked")

        ("input-file", po::value<std::string>(&inputFile), "Read the sets of words to accept and to reject from the given file. If not set, the words are randomly generated (see number-words). The file is composed of two blocks separated by a line with exactly five =. The first block is the set of words to accept while the second block is the set of words to reject. Every word must be on its own line (an empty line is considered as the empty word)")

        ("to-dot", po::bool_switch(&toDot), "If present, the program creates the DOT file describing the constructed DFA. If output-file is not set, the file is outputed in the terminal")
        ("output-file", po::value<std::string>(&outputFile), "If at least one option among to-dot (...) is present, the corresponding outputs are written in files named 'output-file.extension' with the correct extension")

        ("random-benchmarks", po::bool_switch(&rand_bench), "If set, the program executes the random benchmarks")
        ("number-generations", po::value<unsigned int>(&nGenerations)->default_value(50), "If random-benchmarks is set, use this option to change the number of generated samples set by size. By default, 50")

        ("min-size", po::value<unsigned int>(&minSize)->default_value(1), "If random-benchmarks is set, use this option to change the minimum size of the generated samples set. By default, 1")
        ("max-size", po::value<unsigned int>(&maxSize)->default_value(10), "If random-benchmarks is set, use this option to change the maximum size of the generated samples set. By default, 10")

        ("number-words", po::value<unsigned int>(&numberWords)->default_value(10)->notifier([](unsigned int i) {
                if (i < 1)
                    throw std::runtime_error("number-words must be greater or equal to 1");
                }
            ), "If input-file is not set (or if a benchmark is activated), the program generates randomly this number of words. The words are randomly splitted between the words to accept and to words to reject")

        ("word-size", po::value<unsigned int>(&wordSize)->default_value(10), "Use this option to change the size of randomly generated words. By default, 10")
        ("min-word-size", po::value<unsigned int>(&minWordSize), "Use this option to change the minimal size of randomly generated words. If set, max-word-size must be set too and word-size is ignored")
        ("max-word-size", po::value<unsigned int>(&maxWordSize), "Use this option to change the maximal size of randomly generated words. If set, min-word-size must be set too and word-size is ignored")
        ("alphabet-size", po::value<unsigned int>(&alphabetSize)->default_value(10), "Use this option to change the size of the alphabet used to randomly generated the samples. By default, 10")
        ("probability-accepted", po::value<double>(&probabilityAccepted)->default_value(0.5)->notifier([](double i) { if (!(0 <= i && i <= 1)) {throw std::runtime_error("--probability-accepted must be in [0, 1]");}}), "Use this option to change the probability that a generated word is added to the set of words that the DFA must accept. The probability that a word must be rejected is 1 minus this probability. By default, 0.5")

        ("time-limit", po::value<unsigned int>(&timeLimit)->default_value(300), "The time limit, in seconds, the program has to find the best possible solution. If 0, there is no time limit. By default, 300s")
    ;

    po::positional_options_description posopt;
    posopt.add("method", 1);
    posopt.add("number-states", 1);

    po::variables_map variables;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(posopt).run(), variables);

    // First, we check if the help is needed.
    // We don't want to send an error message when the user doesn't know what to do, are we?
    if (variables.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }

    try
    {
        po::notify(variables);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << "\n";
        std::cerr << desc << "\n";
        std::cerr << e.what() << "\n";
        return 2;
    }

    // We check that it's possible to generate enough different words, if we must randomly generate them
    if (!variables.count("input-file")) {
        if (variables.count("min-word-size")) {
            if (variables.count("max-word-size")) {
                unsigned int nPossibilities = 0;
                for (unsigned int size = minWordSize ; size <= maxWordSize && nPossibilities < numberWords ; size++) {
                    nPossibilities += std::pow(alphabetSize, size);
                }

                if (nPossibilities < numberWords) {
                    std::cerr << "Impossible to generate enough words with the given min-word-size, max-word-size and alphabet-size.\n";
                    return 5;
                }
            }
            else {
                std::cerr << "If min-word-size is set, max-word-size must be set. Use -h or --help to see the help.\n";
                return 4;
            }
        }
        else if (variables.count("word-size")) {
            if (std::pow(alphabetSize, wordSize) < numberWords) {
                std::cerr << "Impossible to generate enough words with the given word-size and alphabet-size.\n";
                return 6;
            }
        }
    }

    if (rand_bench) {
        if (variables.count("min-word-size")) {
            random_benchmark(minSize, maxSize, nGenerations, minWordSize, maxWordSize, alphabetSize, probabilityAccepted, timeLimit, verbose);
        }
        else {
            random_benchmark(minSize, maxSize, nGenerations, wordSize, wordSize, alphabetSize, probabilityAccepted, timeLimit, verbose);
        }
    }
    else {
        if (!variables.count("method")) {
            std::cerr << "The method must be provided if a benchmarks suite is not enable. Please see the help: \n";
            std::cerr << desc << "\n";
            return 3;
        }
        std::set<std::string> Sp, Sm;

        if (variables.count("input-file")) {
            passive::readFromFile(inputFile, Sp, Sm);
            print_Sp_Sm(Sp, Sm);
        }
        else {
            if (variables.count("min-word-size")) {
                passive::generateRandomly(numberWords, minWordSize, maxWordSize, alphabetSize, Sp, Sm, probabilityAccepted);
            }
            else {
                passive::generateRandomly(numberWords, wordSize, wordSize, alphabetSize, Sp, Sm, probabilityAccepted);
            }
            print_Sp_Sm(Sp, Sm);
        }

        std::set<std::string> S;
        S = passive::computeS(Sp, Sm);
        
        std::set<std::string> prefixes = passive::computePrefixes(S);
        std::set<char> alphabet = passive::computeAlphabet(S);

        std::unique_ptr<passive::Method> method;
        if (variables.count("number-states")) {
            method = call_method_fixed_n(Sp, Sm, S, prefixes, alphabet, n, choice);
        }
        else {
            long double timeTaken = 0;
            bool succes;
            std::tie(method, succes) = passive::constructMethod(choice, Sp, Sm, S, prefixes, alphabet, std::chrono::seconds(timeLimit), &timeTaken);
            if (succes) {
                std::cout << "We found the best possible DFA\n";
            }
            else {
                std::cout << "Timeout: not enough time to build the optimal DFA. We still show information about the best DFA found.\n";
            }
            std::cout << "Total time: " << timeTaken << "\n";
            std::cout << "Number of states: " << method->numberOfStates() << "\n";
        }

        if (!method || !method->hasSolution()) {
            std::cout << "Check that Sp and Sm are disjoint.\n";
            return 7;
        }

        if (toDot) {
            auto dfa = method->constructDFA();

            if (variables.count("output-file")) {
                std::ofstream out(outputFile + ".dot");
                out << dfa->to_dot();
            }
            else {
                std::cout << dfa->to_dot() << "\n";
            }
        }
    }

    return 0;
}