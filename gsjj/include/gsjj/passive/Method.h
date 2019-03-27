#pragma once

#include <set>
#include <string>
#include <memory>
#include <chrono>
#include <atomic>

#include "gsjj/DFA.h"

namespace gsjj {
    /**
     * Defines the passive methods for automatically learning deterministic finite automata.
     * 
     * These methods receive a sample \f$S = (S_+, S_-)\f$ where
     *  - \f$S_+\f$ is the set of words the automaton must accept
     *  - \f$S_-\f$ is the set of words the automaton must reject
     * 
     * The words that are not in \f$S_+\f$ nor in \f$S_-\f$ can be either accepted or rejected (depending on the generated DFA)
     */
    namespace passive {
        /**
         * An array with every possible method choice. Useful to iterate over the methods
         */
        const std::array<std::string, 8> allMethods = {
            "biermann",
            "unary",
            "binary",
            "heule",
            "neider",
            "unaryNonCNF",
            "binaryNonCNF",
            "heuleNonCNF"
        };

        /**
         * The base class of all passive methods
         */
        class Method {
        public:

        public:
            Method() = delete;
            virtual ~Method();

            /**
             * Solves the SAT/SMT problem induced by the method.
             *
             * Il also creates every variable and clause needed
             * @return True iff the problem is solvable
             */
            virtual bool solve() = 0;

            /**
             * If solve() has been called and if there exists a solution, return True. Otherwise, return false
             * 
             * @return True iff the SAT problem has a solution
             */
            virtual bool hasSolution() const = 0;

            /**
             * Gives the number of states of the prospected DFA (the n)
             * 
             * @return The number of states
             */
            unsigned int numberOfStates() const;

            /**
             * Constructs the DFA induced by the variables according to the method.
             * 
             * If the SAT/SMT problem has not yet been solved, the function automatically solves it.
             * If the SAT/SMT problem is unsolvable, returns a null pointer.
             * @return An unique_ptr to the constructed DFA or an empty pointer if the problem is not solvable
             */
            virtual std::unique_ptr<DFA<char>> constructDFA() = 0;

            /**
             * Prints the values of the variables, UNSAT or NOT SOLVED.
             * 
             * If solve() has not yet been called, prints NOT SOLVED.
             * Else if the problem is not satisfiable, prints UNSAT.
             * Else, prints SAT and the values of the variables.
             */
            virtual void printVariables() const = 0;

            /**
             * Gives the CPU time taken to solve the problem.
             * 
             * @return The CPU time taken by the solver
             */
            double timeToSolve() const;

            /**
             * Sets the stop trigger for this method.
             * 
             * Since we use three different solvers and each has its own way to stop (CVC4 uses a function to set a time limit, Maple is in C++ so we can use an atomic variable and Limboole is in C and we must use a pointer), we must provide each possibility
             * @param timeLimit The time in seconds the method can take. It's used by SMT methods.
             * @param stopTrigger The atomic_bool variable. Once set to true, the method will stop. It's used by CNF methods.
             * @param stopPointer The pointer variable. Once the value is set to true, the method will stop. It's used by NonCNF methods.
             */
            virtual void setStopTrigger(const std::chrono::seconds &timeLimit, std::atomic_bool &stopTrigger, const bool *stopPointer) = 0;

        protected:
            /**
             * The set of words to accept \f$S_+\f$
             */
            const std::set<std::string> m_Sp;
            /**
             * The set of words to reject \f$S_-\f$
             */
            const std::set<std::string> m_Sm;
            /**
             * The number of states in the DFA to construct
             */
            const unsigned int m_numberStates;
            /**
             * In the code, \f$S = S_+ \cup S_-\f$
             */
            const std::set<std::string> m_S;
            /**
             * \f$Pref(S_+ \cup S_-)\f$
             */
            const std::set<std::string> m_prefixes;
            /**
             * \f$\Sigma\f$
             */
            const std::set<char> m_alphabet;

            bool m_triedSolve;
            bool m_hasSolution;

            double m_cpuTimeStart;
            double m_cpuTimeEnd;

        protected:
            /**
             * Creates the method.
             * 
             * @param SpSet The set of words to accept
             * @param SmSet The set of words to reject
             * @param SSet The union of SpSet and SmSet
             * @param prefixesSet The prefixes of S
             * @param alphabetSet The used alphabet
             * @param n The number of states in the prospected DFA
             */
            Method(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n);
        };
    }
}