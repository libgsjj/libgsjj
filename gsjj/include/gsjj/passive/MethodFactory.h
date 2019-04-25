#pragma once

#include <memory>
#include <string>
#include <map>
#include <functional>

#include "gsjj/passive/Method.h"

namespace gsjj {
    namespace passive {
        /**
         * The Factory Builder design pattern adapted to our methods.
         * 
         * The methods must register themselves to this factory.
         * 
         * Some implementation details are inspired from https://dzone.com/articles/factory-with-self-registering-types
         */
        class MethodFactory {
        public:
            /**
             * The type the function create will return.
             */
            using MethodPtr = std::unique_ptr<Method>;
            /**
             * The type of the constructor the registerMethod must receive
             */
            using MethodConstructor = std::function<MethodPtr(
                const std::set<std::string>& Sp,
                const std::set<std::string>& Sm,
                const std::set<std::string>& S,
                const std::set<std::string>& prefixes,
                const std::set<char>& alphabet,
                unsigned int n
            )>;

        public:
            MethodFactory() = delete; // We don't want to create an instance of this class

            /**
             * Registers a method
             * @param name The name of the method to register
             * @param constructor A pointer to the constructor of the method
             * @return True iff it was possible to register the method
             * @sa gsjj::passive::RegisterInFactory
             */
            static bool registerMethod(const std::string &name, MethodConstructor const &constructor);

            /**
             * Creates a unique_ptr to a method.
             * 
             * The method must have been registered before!
             * 
             * Please use constructMethod instead of this function
             * 
             * @param name The name of the method to construct
             * @param n The number of states
             * @param Sp The \f$S_+\f$ set
             * @param Sm The \f$S_-\f$ set
             * @param S The \f$S = S_+ \cup S_-\f$
             * @param prefixes The prefixes set
             * @param alphabet The alphabet
             * @sa registerMethod, constructMethod
             */
            static MethodPtr create(const std::string &name, unsigned int n, const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::set<std::string> &S, const std::set<std::string> &prefixes, const std::set<char> &alphabet);

        private:
            static std::map<std::string, MethodConstructor> s_methodsMap;
        };

        /**
         * Helps to register a method in the factory.
         * 
         * To register a method (through this helper), the method must inherit from RegisterInFactory (and specify the typename). Due to the annoying (in this context) compiler's optimisations, the method must also have a line that uses s_registered. For example, adding s_registered = s_registered in any function of the method makes everything working as intented.
         * 
         * Each class inheriting this class must implement a function with the following signature :
         * 
         *      static std::string getFactoryName()
         * 
         * The constructor of the class must have the same signature than the built-in methods' constructors (see, for example, gsjj::passive::SMT::BiermannMethod::BiermannMethod)
         * @tparam T The type of the method to register
         */
        template <typename T>
        class RegisterInFactory {
        protected:
            /**
             * Useful to make sure the template is correctly used by the compiler.
             * 
             * Please add a line such as
             * 
             *      s_registered = s_registered;
             * 
             * anywhere in the class that inherits this helper.
             * 
             * By default, this variable asks to map T::getFactoryName() to a way to construct T.
             */
            static bool s_registered;
        };

        template<typename T>
        bool RegisterInFactory<T>::s_registered = MethodFactory::registerMethod(T::getFactoryName(),
            [](const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::set<std::string> &S, const std::set<std::string> &prefixes, const std::set<char> &alphabet, unsigned int n) -> MethodFactory::MethodPtr {
                class MethodImplemented : public T {
                public:
                    MethodImplemented(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n) : T(SpSet, SmSet, SSet, prefixesSet, alphabetSet, n) {}
                };

                return std::move(std::make_unique<MethodImplemented>(Sp, Sm, S, prefixes, alphabet, n));
            }
        );

        /**
         * Computes the alphabet and the prefixes set from Sp and Sm and constructs a method for a fixed number of states.
         * 
         * Sp and Sm must be disjoint.
         * @param name The name of the method to construct
         * @param n The number of states
         * @param Sp The \f$S_+\f$ set
         * @param Sm The \f$S_-\f$ set
         * @return A method to construct the DFA for the sample \f$(S_+, S_-)\f$
         * @warning S can not be empty (and must be coherent with Sp and Sm)
         */
        std::unique_ptr<Method> constructMethod(const std::string &name, unsigned int n, const std::set<std::string> &Sp, const std::set<std::string> &Sm);

        /**
         * Constructs a method for a fixed number of states.
         * 
         * Sp and Sm must be disjoint.
         * @param name The name of the method to construct
         * @param n The number of states
         * @param Sp The \f$S_+\f$ set
         * @param Sm The \f$S_-\f$ set
         * @param S The \f$S = S_+ \cup S_-\f$ set. See passive::computeS
         * @param prefixes The \f$Pref(S)\f$ set. See passive::computePrefixes
         * @param alphabet The alphabet. See computeAlphabet
         * @return A method to construct the DFA for the sample \f$(S_+, S_-)\f$
         * @warning S can not be empty (and must be coherent with Sp and Sm)
         */
        std::unique_ptr<Method> constructMethod(const std::string &name, unsigned int n, const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::set<std::string> &S, const std::set<std::string> &prefixes, const std::set<char> &alphabet);

        /**
         * Computes the alphabet and the prefixes set from Sp and Sm and constructs a method with the smallest possible number of states.
         * 
         * This uses a linear search to find the minimal number of states
         * 
         * Sp and Sm must be disjoint.
         * @param name The name of the method to construct
         * @param Sp The \f$S_+\f$ set
         * @param Sm The \f$S_-\f$ set
         * @param timeLimit The time in seconds the program can take to find the best possible method.
         * @param timeTaken If not nullptr, the total time used to solve the different formulas is written. It does NOT give the full CPU time to find the best number of states, juste the time used by the SAT/SMT solvers!
         * @return A method to construct the DFA for the sample \f$(S_+, S_-)\f$ and a boolean. The boolean is true iff the function did not reach the time limit.
         * @warning S can not be empty (and must be coherent with Sp and Sm)
         */
        std::pair<std::unique_ptr<Method>, bool> constructMethod(const std::string &name, const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::chrono::seconds &timeLimit = std::chrono::seconds(0), long double *timeTaken = nullptr);

        /**
         * Constructs a method with the smallest possible number of states.
         * 
         * This uses a linear search to find the minimal number of states
         * 
         * Sp and Sm must be disjoint.
         * @param name The name of the method to construct
         * @param Sp The \f$S_+\f$ set
         * @param Sm The \f$S_-\f$ set
         * @param S \f$S = S_+ \cup S_-\f$. See passive::computeS
         * @param prefixes The set of prefixes of \f$S\f$. See passive::computePrefixes
         * @param alphabet The alphabet of \f$S\f$. See passive::computeAlphabet
         * @param timeLimit The time in milliseconds the program can take to find the best possible method.
         * @param timeTaken If not nullptr, the total time used to solve the different formulas is written. It does NOT give the full CPU time to find the best number of states, juste the time used by the SAT/SMT solvers!
         * @return A method to construct the DFA for the sample \f$(S_+, S_-)\f$ and a boolean. The boolean is true iff the function did not reach the time limit.
         * @warning S can not be empty (and must be coherent with Sp and Sm)
         */
        std::pair<std::unique_ptr<Method>, bool> constructMethod(const std::string &name, const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::set<std::string> &S, const std::set<std::string> &prefixes, const std::set<char> &alphabet, const std::chrono::seconds &timeLimit = std::chrono::seconds(0), long double *timeTaken = nullptr);
    }
}