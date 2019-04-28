#pragma once

#include <ostream>
#include <atomic>

#include "gsjj/passive/Method.h"

// Since limboole is written in C, we must force the compiler
extern "C" {
    #include "limboole/limboole.h"
}

namespace gsjj {
    namespace passive {
        /**
         * Contains every method that relies on a SAT solver that works on any boolean formula.
         * 
         * These methodes use limboole.
         */
        namespace NonCNF {
            /**
             * Base class for every method that relies on a non-CNF SAT solver.
             * 
             * It creates a non-CNF SAT solver
             */
            class SATNonCNFMethod : public Method {
            public:
                SATNonCNFMethod() = delete;
                virtual ~SATNonCNFMethod();

                virtual bool solve() override;

                virtual std::unique_ptr<DFA<char>> constructDFA() override;

                virtual void setStopTrigger(const std::chrono::seconds &timeLimit, std::atomic_bool &stopTrigger, const bool *stopPointer) override;

            protected:
                /**
                 * Gives the name of the file in which the boolean formula must be written
                 * @return The name of the file
                 */
                virtual std::string getFileName() const = 0;

                /**
                 * Reads the output of the SAT solver and checks if the formula was satisfiable or not
                 * @return True iff the formula was satisfiable
                 */
                bool isSatisfiable() const;

                /**
                 * Creates the boolean formula (must not be in CNF) and writes it in the stream
                 * @param stream The output stream in which to write
                 * @return True iff it is possible to create the formula
                 */
                virtual bool createFormula(std::ostream &stream) = 0;

                /**
                 * Effectively creates the DFA.
                 * 
                 * If this function is called, the boolean formula is satisfiable. The output of the SAT solver is given in the stream
                 */
                virtual std::unique_ptr<DFA<char>> toDFA(std::istream &stream) const = 0;

                SATNonCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n);

            protected:
                const bool *mustStop;

            private:
                /**
                 * The name of the file in which to write the boolean formula
                 */
                std::string getInputFile() const;
                /**
                 * The name of the file in which the SAT solver writes
                 */
                std::string getOutputFile() const;
            };
        }
    }
}
