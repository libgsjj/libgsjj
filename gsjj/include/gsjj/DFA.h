/**
 * \file DFA.h
 * 
 * This file declares and implements the templated class DFA.
 */

#pragma once

// These container hash the keys, therefore the average complexity is constant
#include <unordered_set>
// These container use a red-black tree, therefore the complexity is logarithmic
#include <map>
#include <vector>
#include <list>

#include <string>
#include <sstream>

/**
 * The namespace of the library
 */
namespace gsjj {
    /**
     * A Deterministic Finite Automaton.
     * 
     * A DFA is encoded by a 5-tuple \f$(Q, \Sigma, q_0, \delta, F)\f$ with:
     *      - \f$Q\f$, the set of states. Here, a state is represented by an integer (starting from 0)
     *      - \f$\Sigma\f$, the alphabet. Here, the alphabet is all the values that can be coded in a given C++ type (for example, a char)
     *      - \f$q_0\f$, the initial state
     *      - \f$\delta\f$, the transition function such that delta: \f$Q \times Sigma \rightarrow Q: \delta(q, a) = p\f$
     *      - \f$F\f$, the set of final/accepting states
     * 
     * We make the assumption that \f$Q\f$ cannot be empty (because there must be at least the initial state).
     * 
     * @tparam Sigma The type of the alphabet (for example, char).
     */
    template<typename Sigma>
    class DFA {
    public:
        /**
         * Constructs the DFA.
         * 
         * @param initial The initial state
         * @param isAccepting Whether the initial state is accepting or not
         */
        DFA(unsigned int initial, bool isAccepting = false) {
            m_states.insert(initial);
            m_initialState = initial;
            if (isAccepting) {
                m_acceptingStates.insert(initial);
            }
        }

        /**
         * Adds a state to the DFA.
         * 
         * The functions checks that the state does not yet exist.
         * @param state The number of the state.
         * @param isAccepting Whether the new state is accepting or not
         */
        void addState(unsigned int state, bool isAccepting = false) {
            m_states.insert(state); // The check if the state is already in the set is automatically done in the insert function
            if (isAccepting) {
                m_acceptingStates.insert(state);
            }
        }

        /**
         * Adds a transition to the DFA.
         * 
         * If the DFA is in the state p and reads the given symbol, it goes to the state q.
         * The function checks that delta(p, symbol) does not yet exist and that p and q exist.
         * 
         * @param p The state where the transition starts
         * @param symbol The symbol to read to activate the transition
         * @param q The state where the transition ends
         */
        void addTransition(unsigned int p, Sigma symbol, unsigned int q) {
            if (m_states.find(p) == m_states.end()) {
                std::ostringstream error;
                error << p << " is not known when adding the transition " << p << " --" << symbol << "--> " << q;
                throw std::out_of_range(error.str());
            }
            else if (m_states.find(q) == m_states.end()) {
                std::ostringstream error;
                error << q << " is not known when adding the transition " << p << " --" << symbol << "--> " << q;
                throw std::out_of_range(error.str());
            }

            m_transitions.emplace(std::make_pair(p, symbol), q);
        }

        /**
         * Tests whether the word is accepted by the DFA
         * @param word The word to test
         * @return True iff the word is accepted by the DFA
         */
        bool isAccepted(const std::list<Sigma>& word) {
            unsigned int state = m_initialState;
            for (const Sigma &a : word) {
                auto tran = m_transitions.find(std::make_pair(state, a));

                if (tran != m_transitions.end()) {
                    state = tran->second;
                }
                else {
                    return false;
                }
            }

            return m_acceptingStates.find(state) != m_acceptingStates.end();
        }

        /**
         * Tests whether the word is accepted by the DFA
         * @param word The word to test
         * @return True iff the word is accepted by the DFA
         */
        bool isAccepted(const std::vector<Sigma> &word) {
            unsigned int state = m_initialState;
            for (const Sigma &a : word) {
                auto tran = m_transitions.find(std::make_pair(state, a));

                if (tran != m_transitions.end()) {
                    state = tran->second;
                }
                else {
                    return false;
                }
            }

            return m_acceptingStates.find(state) != m_acceptingStates.end();
        }

        /**
         * Returns the number of states in this DFA
         * @return The number of states
         */
        unsigned int getNumberOfStates() const {
            return m_states.size();
        }

        /**
         * Constructs a string that is the representation of the DFA in the DOT language.
         * @return The string
         */
        std::string to_dot() const {
            std::ostringstream dot;
            dot << "digraph deterministic_finite_automaton {\n";
            dot << "\trankdir=LR;\n";
            dot << "\tsize=\"8.5\"\n";
            process_states_dot(dot);
            process_transitions_dot(dot);
            dot << "}\n";
            return dot.str();
        }

    private:
        /**
         * The set \f$Q\f$.
         */
        std::unordered_set<unsigned int> m_states;
        /**
         * The initial state \f$q_0\f$.
         */
        unsigned int m_initialState;
        /**
         * The \f$\delta\f$ function.
         */
        std::map<std::pair<unsigned int, Sigma>, int> m_transitions;
        /**
         * The set \f$F\f$.
         */
        std::unordered_set<unsigned int> m_acceptingStates;

    private:
        /**
         * Processes the states of the DFA to create a dot file.
         * \param dot The stringstream in which the function must write
         */
        void process_states_dot(std::ostringstream& dot) const {
            // We must add one state to create the arrow indicating the initial state
            dot << "\tnode [shape=none, label=\"\"] qi\n";
            for (int state : m_states) {
                dot << "\tnode [shape=";
                // Accepting states are double circled
                if (m_acceptingStates.find(state) != m_acceptingStates.end()) {
                    dot << "doublecircle";
                }
                else {
                    dot << "circle";
                }

                dot << ", label=\"q" << state << "\"] q" << state << ";\n";
            }
        }

        /**
         * \brief Processes the transitions of the DFA to create a dot file.
         * \param dot The stringstream in which the function must write
         */
        void process_transitions_dot(std::ostringstream &dot) const {
            // The arrow indicating the initial state
            dot << "\tqi -> q" << m_initialState << ";\n";

            for (auto transition : m_transitions) {
                unsigned int start = transition.first.first;
                unsigned int end = transition.second;
                Sigma symbol = transition.first.second;

                dot << "\tq" << start << " -> q" << end << " [label=\"" << symbol << "\"];\n";
            }
        }
    };
}