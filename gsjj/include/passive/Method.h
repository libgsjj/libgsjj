#pragma once

#include <set>
#include <string>
#include <memory>
#include <chrono>

#include "DFA.h"

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
         * Enumerates every available method.
         * 
         * Please use Method::constructMethod to build the method according to the method of your choice
         */
        enum class Methods {
            /** The Biermann and Feldman method */
            BIERMANN = 0,
            /** The Grinchtein, Leucker and Piterman's unary method with a CNF SAT solver */
            UNARY,
            /** The Grinchtein, Leucker and Piterman's binary method with a CNF SAT solver */
            BINARY,
            /** The Heule and Verwer method with a CNF SAT solver */
            HEULEVERWER,
            /** The Neider and Jansen method */
            NEIDERJANSEN,
            /** The Grinchtein, Leucker and Piterman's unary method with a non-CNF SAT solver */
            UNARY_NON_CNF,
            /** The Grinchtein, Leucker and Piterman's binary method with a non-CNF SAT solver */
            BINARY_NON_CNF,
            /** The Heule and Verwer method with a non-CNF SAT solver */
            HEULE_NON_CNF
        };
        /**
         * An array with every possible method choice. Useful to iterate over the methods
         */
        const std::array<Methods, 8> allMethods = {
            Methods::BIERMANN,
            Methods::UNARY,
            Methods::BINARY,
            Methods::HEULEVERWER,
            Methods::NEIDERJANSEN,
            Methods::UNARY_NON_CNF,
            Methods::BINARY_NON_CNF,
            Methods::HEULE_NON_CNF
        };

        /**
         * A map associating every method with a name easily readable for a human
         */
        const std::map<Methods, std::string> methodsNames = {
            {Methods::BIERMANN, "biermann"},
            {Methods::UNARY, "unary"},
            {Methods::BINARY, "binary"},
            {Methods::HEULEVERWER, "heule"},
            {Methods::NEIDERJANSEN, "neider"},
            {Methods::UNARY_NON_CNF, "unary (non CNF)"},
            {Methods::BINARY_NON_CNF, "binary (non CNF)"},
            {Methods::HEULE_NON_CNF, "heule (non CNF)"}
        };

        /**
         * The base class of all passive methods
         */
        class Method {
        public:
            /**
             * Computes the alphabet and the prefixes set from Sp and Sm and constructs a method for a fixed number of states.
             * 
             * Sp and Sm must be disjoint.
             * @param method The choice of the method to construct
             * @param n The number of states
             * @param Sp The \f$S_+\f$ set
             * @param Sm The \f$S_-\f$ set
             * @return A method to construct the DFA for the sample \f$(S_+, S_-)\f$
             */
            static std::unique_ptr<Method> constructMethod(Methods method, unsigned int n, const std::set<std::string> &Sp, const std::set<std::string> &Sm);

            /**
             * Constructs a method for a fixed number of states.
             * 
             * Sp and Sm must be disjoint.
             * @param method The choice of the method to construct
             * @param n The number of states
             * @param Sp The \f$S_+\f$ set
             * @param Sm The \f$S_-\f$ set
             * @param S The \f$S = S_+ \cup S_-\f$ set. See passive::computeS
             * @param prefixes The \f$Pref(S)\f$ set. See passive::computePrefixes
             * @param alphabet The alphabet. See computeAlphabet
             * @return A method to construct the DFA for the sample \f$(S_+, S_-)\f$
             */
            static std::unique_ptr<Method> constructMethod(Methods method, unsigned int n, const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::set<std::string> &S, const std::set<std::string> &prefixes, const std::set<char> &alphabet);

            /**
             * Computes the alphabet and the prefixes set from Sp and Sm and constructs a method with the smallest possible number of states.
             * 
             * This uses a binary search to find the minimal number of states
             * 
             * Sp and Sm must be disjoint.
             * @param method The choice of the method to construct
             * @param Sp The \f$S_+\f$ set
             * @param Sm The \f$S_-\f$ set
             * @param timeLimit The time in seconds the program can take to find the best possible method. If the time limit is reached, the function returns the best method found so far.
             * @param timeTaken If not nullptr, the total time used to solve the different formulas is written. It does NOT give the full CPU time to find the best number of states, juste the time used by the SAT/SMT solvers!
             * @return A method to construct the DFA for the sample \f$(S_+, S_-)\f$ and a boolean. The boolean is true iff the function did not reach the time limit.
             */
            static std::pair<std::unique_ptr<Method>, bool> constructMethod(Methods method, const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::chrono::seconds &timeLimit = std::chrono::seconds(0), long double *timeTaken = nullptr);

            /**
             * Constructs a method with the smallest possible number of states.
             * 
             * This uses a binary search to find the minimal number of states
             * 
             * Sp and Sm must be disjoint.
             * @param method The choice of the method to construct
             * @param Sp The \f$S_+\f$ set
             * @param Sm The \f$S_-\f$ set
             * @param S \f$S = S_+ \cup S_-\f$. See passive::computeS
             * @param prefixes The set of prefixes of \f$S\f$. See passive::computePrefixes
             * @param alphabet The alphabet of \f$S\f$. See passive::computeAlphabet
             * @param timeLimit The time in milliseconds the program can take to find the best possible method. If the time limit is reached, the function returns the best method found so far.
             * @param timeTaken If not nullptr, the total time used to solve the different formulas is written. It does NOT give the full CPU time to find the best number of states, juste the time used by the SAT/SMT solvers!
             * @return A method to construct the DFA for the sample \f$(S_+, S_-)\f$ and a boolean. The boolean is true iff the function did not reach the time limit.
             */
            static std::pair<std::unique_ptr<Method>, bool> constructMethod(Methods method, const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::set<std::string> &S, const std::set<std::string> &prefixes, const std::set<char> &alphabet, const std::chrono::seconds &timeLimit = std::chrono::seconds(0), long double *timeTaken = nullptr);

        public:
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

            double m_cpuTimeStart;
            double m_cpuTimeEnd;
        };
    }
}