#pragma once

#include "mtl/Vec.h"
#include "core/SolverTypes.h"
#include "simp/SimpSolver.h"

#include "passive/Method.h"

namespace passive {
    /**
     * Contains every method that relies on a SAT solver with CNF clauses.
     * 
     * These methodes use Maple.
     */
    namespace CNF {
        /**
         * Base class for every method that relies on a SAT solver.
         * 
         * It creates a SAT solver and defines some functions to easily create a new variable and new clauses.
         */
        class SATCNFMethod : public Method {
        public:
            SATCNFMethod(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n, std::atomic_bool &stopTrigger);
            virtual ~SATCNFMethod();

            bool solve() override;

            bool hasSolution() const override;

            std::unique_ptr<DFA<char>> constructDFA() override;

            void printVariables() const override;
        
        protected:
            /**
             * Creates a new variable in the SAT solver.
             * 
             * A Minisat::Var is actually just an integer
             * @return The new variable
             */
            Minisat::Var newVariable();
            /**
             * Adds a new clause to the SAT solver.
             * 
             * It creates a copy of the given clause. Therefore, one can use the same Minisat::vec to construct every clause.
             * @param clause The clause to add
             */
            void addClause(const Minisat::vec<Minisat::Lit> &clause);

            /**
             * Creates all needed variables for the method
             */
            virtual void createVariables() = 0;
            /**
             * Creates all needed clauses for the method
             */
            virtual void createClauses() = 0;

            /**
             * Effectively creates the DFA from the values of the variables in the given model
             * @param model The model
             * @return An unique_ptr to the constructed DFA
             */
            virtual std::unique_ptr<DFA<char>> toDFA(const Minisat::vec<Minisat::lbool> &model) const = 0;

        private:
            /**
             * The SAT solver.
             * 
             * It's a pointer because the copy/move constructor is disabled and we need to reset the solver
             */
            std::unique_ptr<Minisat::SimpSolver> m_solver;
            
            bool m_hasSolution;

            std::atomic_bool &mustStop;
        };
    }
}