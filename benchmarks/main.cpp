#include <iostream>
#include <istream>
#include <array>

#include <sys/resource.h>

#include <boost/program_options.hpp>
#include <cvc4/cvc4.h>

#include "gsjj/passive/Method.h"
#include "gsjj/passive/utils.h"
#include "gsjj/passive/MethodFactory.h"

#include "LFDFA.h"

namespace po = boost::program_options;
using namespace gsjj;

/**
 * Prints Sp and Sm in the stream
 * @param Sp The Sp set
 * @param Sm The Sm set
 * @param stream The stream in which to write the sample
 */
void print_Sp_Sm(const std::set<std::string> &Sp, const std::set<std::string> &Sm, std::ostream &stream) {
    for (const auto &s : Sp) {
        stream << s << "\n";
    }
    stream << "=====\n";
    for (const auto &s : Sm) {
        stream << s << "\n";
    }
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

/**
 * Execute a method on the sample described in the input file.
 * 
 * It prints the timeTaken to find the optimal DFA (or something around the timeLimit if it did not have enough time).
 * @param method The method to execute
 * @param input The input file
 * @param timeLimit The time limit (in seconds)
 * @return True iff the method could find the optimal DFA
 */
bool benchmarks(const std::string &method, const std::string &input, unsigned int timeLimit) {
    std::set<std::string> Sp, Sm, S, prefixes;
    std::set<char> alphabet;
    passive::readFromFile(input, Sp, Sm);
    S = passive::computeS(Sp, Sm);
    prefixes = passive::computePrefixes(S);
    alphabet = passive::computeAlphabet(S);
    long double timeTaken = 0;
    std::unique_ptr<passive::Method> ptr;
    bool success;
    std::tie(ptr, success) = passive::constructMethod(method, Sp, Sm, S, prefixes, alphabet, std::chrono::seconds(timeLimit), &timeTaken);
    std::cout << timeTaken << "\n";
    return success;
}

/**
 * Execute a method on the sample described by a loop-free DFA written in the input file.
 * 
 * It prints the timeTaken to find the optimal DFA (or something around the timeLimit if it did not have enough time).
 * @param method The method to execute
 * @param input The input file
 * @param timeLimit The time limit (in seconds)
 * @return True iff the method could find the optimal DFA
 */
bool benchmarksLoopFree(const std::string &method, const std::string &input, unsigned int timeLimit) {
    auto dfa = LFDFA::loadFromFile(input);
    std::set<std::string> Sp, Sm;
    dfa->getSets(Sp, Sm);
    auto S = passive::computeS(Sp, Sm);
    auto alphabet = passive::computeAlphabet(S);
    auto prefixes = passive::computePrefixes(S);

    long double timeTaken = 0;
    std::unique_ptr<passive::Method> ptr;
    bool success;
    std::tie(ptr, success) = passive::constructMethod(method, Sp, Sm, S, prefixes, alphabet, std::chrono::seconds(timeLimit), &timeTaken);
    std::cout << timeTaken << "\n";
    return success;
}

int main(int argc, char** argv) {
    std::string choice;
    std::string inputFile, outputFile;
    bool toDot, verbose, generateSample, bench, loopFree;
    unsigned int n, numberWords, wordSize, minWordSize, maxWordSize, alphabetSize, timeLimit;
    double probabilityAccepted;

    po::options_description desc("Benchmarks for learning DFA algorithms. For the moment, it only works for passive algorithms.");
    desc.add_options()
        ("help,h", "produce help message")
        ("verbose,v", po::bool_switch(&verbose), "If set, the program outputs more information")

        ("generate-sample", po::bool_switch(&generateSample), "If set, the program generates a sample. See number-words, word-size, min-word-size, max-word-size, probability-accepted and output-file for configuration")

        ("method", po::value<std::string>(&choice), "The method to use [MANDATORY if random-benchmarks is not set]. The '--method' part is not necessary (positional argument)")
        ("number-states,n", po::value<unsigned int>(&n)->notifier([](unsigned int i) {
                if (i < 1) {
                    throw std::runtime_error("--number-states (or -n) must be greater or equal to 1");
                }
            }), "If set, the program tries to construct a DFA with exactly n states. If not set, the minimal n is seeked")

        ("input-file", po::value<std::string>(&inputFile), "Read the sets of words to accept and to reject from the given file. If not set, the words are randomly generated (see number-words). The file is composed of two blocks separated by a line with exactly five =. The first block is the set of words to accept while the second block is the set of words to reject. Every word must be on its own line (an empty line is considered as the empty word)")
        ("loop-free", po::bool_switch(&loopFree), "If set, the input file is processed as a 'kis' file describing a Loop-Free DFA")

        ("to-dot", po::bool_switch(&toDot), "If present, the program creates the DOT file describing the constructed DFA. If output-file is not set, the file is outputed in the terminal")
        ("output-file", po::value<std::string>(&outputFile)->default_value("out"), "If at least one option among to-dot (...) is present, the corresponding outputs are written in files named 'output-file.extension' with the correct extension. If generate-sample is set, the sample is written in the output file.")

        ("benchmarks", po::bool_switch(&bench), "If set, the program executes the given method on the sample in the input file and prints the taken time to the standard output. method and input-file must be set.")

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

        ("time-limit", po::value<unsigned int>(&timeLimit)->default_value(300), "The time limit, in seconds, the program has to find the best possible solution. If 0, there is no time limit. It's possible the program takes more time than the time limit allows because the program needs the reach a point where it checks if it must stops and some computation's steps take time. By default, 300s.")
    ;

    po::positional_options_description posopt;
    posopt.add("method", 1);
    posopt.add("number-states", 1);

    po::variables_map variables;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(posopt).run(), variables);

    // First, we check if the help is needed.
    // We don't want to send an error message when the user doesn't know what to do
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

    if (generateSample) {
        std::set<std::string> Sp, Sm;
        if (variables.count("min-word-size")) {
            passive::generateRandomly(numberWords, minWordSize, maxWordSize, alphabetSize, Sp, Sm, probabilityAccepted);
        }
        else {
            passive::generateRandomly(numberWords, wordSize, wordSize, alphabetSize, Sp, Sm, probabilityAccepted);
        }

        std::ofstream stream(outputFile);
        print_Sp_Sm(Sp, Sm, stream);
        stream.close();
    }
    else if (bench) {
        if (loopFree) {
            if (benchmarksLoopFree(choice, inputFile, timeLimit)) {
                return 0;
            }
			return 1;
        }
        else {
            if (benchmarks(choice, inputFile, timeLimit)) {
                return 0;
            }
            return 1;
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
            if (loopFree) {
                LFDFA::loadFromFile(inputFile)->getSets(Sp, Sm);
            }
            else {
                passive::readFromFile(inputFile, Sp, Sm);
            }
        }
        else {
            if (variables.count("min-word-size")) {
                passive::generateRandomly(numberWords, minWordSize, maxWordSize, alphabetSize, Sp, Sm, probabilityAccepted);
            }
            else {
                passive::generateRandomly(numberWords, wordSize, wordSize, alphabetSize, Sp, Sm, probabilityAccepted);
            }
            print_Sp_Sm(Sp, Sm, std::cout);
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
                std::cout << "Timeout: not enough time to build the optimal DFA.\n";
                return 8;
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

            std::ofstream out(outputFile + ".dot");
            out << dfa->to_dot();
        }
    }

    return 0;
}
