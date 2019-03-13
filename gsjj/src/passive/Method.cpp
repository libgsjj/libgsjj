#include "passive/Method.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <future>

#include "utils/System.h"

#include "passive/SMT/BiermannMethod.h"
#include "passive/SMT/NeiderJansenMethod.h"

#include "passive/CNF/UnaryCNFMethod.h"
#include "passive/CNF/BinaryCNFMethod.h"
#include "passive/CNF/HeuleVerwerCNFMethod.h"

#include "passive/NonCNF/UnaryNonCNFMethod.h"
#include "passive/NonCNF/BinaryNonCNFMethod.h"
#include "passive/NonCNF/HeuleVerwerNonCNFMethod.h"

#include "passive/utils.h"

using namespace Minisat;

std::atomic_bool falseStopTrigger(false);
std::chrono::seconds noLimit(0);
bool falseStopBool;

std::unique_ptr<passive::Method> constructMethodTrigger(passive::Methods method, unsigned int n, const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::set<std::string> &S, const std::set<std::string> &prefixes, const std::set<char> &alphabet, const std::chrono::seconds &timeLimit, std::atomic_bool &stopTrigger, const bool *stopPointer) {
    switch(method) {
    case passive::Methods::BIERMANN:
        return std::make_unique<passive::SMT::BiermannMethod>(Sp, Sm, S, prefixes, alphabet, n, timeLimit);
        break;
    case passive::Methods::UNARY:
        return std::make_unique<passive::CNF::UnaryCNFMethod>(Sp, Sm, S, prefixes, alphabet, n, stopTrigger);
        break;
    case passive::Methods::BINARY:
        return std::make_unique<passive::CNF::BinaryCNFMethod>(Sp, Sm, S, prefixes, alphabet, n, stopTrigger);
        break;
    case passive::Methods::HEULEVERWER:
        return std::make_unique<passive::CNF::HeuleVerwerCNFMethod>(Sp, Sm, S, prefixes, alphabet, n, stopTrigger);
        break;
    case passive::Methods::NEIDERJANSEN:
        return std::make_unique<passive::SMT::NeiderJansenMethod>(Sp, Sm, S, prefixes, alphabet, n, timeLimit);
        break;
    case passive::Methods::UNARY_NON_CNF:
        return std::make_unique<passive::NonCNF::UnaryNonCNFMethod>(Sp, Sm, S, prefixes, alphabet, n, stopPointer);
        break;
    case passive::Methods::BINARY_NON_CNF:
        return std::make_unique<passive::NonCNF::BinaryNonCNFMethod>(Sp, Sm, S, prefixes, alphabet, n, stopPointer);
        break;
    case passive::Methods::HEULE_NON_CNF:
        return std::make_unique<passive::NonCNF::HeuleVerwerNonCNFMethod>(Sp, Sm, S, prefixes, alphabet, n, stopPointer);
        break;
    default:
        throw std::invalid_argument("Unknown method type");
        break;
    }
}

namespace passive {
    std::unique_ptr<Method> Method::constructMethod(Methods method, unsigned int n, const std::set<std::string> &Sp, const std::set<std::string> &Sm) {
        std::set<std::string> S = computeS(Sp, Sm);
        std::set<std::string> prefixes = computePrefixes(S);
        std::set<char> alphabet = computeAlphabet(S);
        
        return std::move(Method::constructMethod(method, n, Sp, Sm, S, prefixes, alphabet));
    }

    std::unique_ptr<Method> Method::constructMethod(Methods method, unsigned int n, const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::set<std::string> &S, const std::set<std::string> &prefixes, const std::set<char> &alphabet) {
        return std::move(constructMethodTrigger(method, n, Sp, Sm, S, prefixes, alphabet, noLimit, falseStopTrigger, &falseStopBool));
    }

    std::pair<std::unique_ptr<Method>, bool> Method::constructMethod(Methods method, const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::chrono::seconds &timeLimit, long double *timeTaken) {
        std::set<std::string> S = computeS(Sp, Sm);
        std::set<std::string> prefixes = computePrefixes(S);
        std::set<char> alphabet = computeAlphabet(S);
        
        return Method::constructMethod(method, Sp, Sm, S, prefixes, alphabet, timeLimit, timeTaken);
    }

    std::pair<std::unique_ptr<Method>, bool> Method::constructMethod(Methods method, const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::set<std::string> &S, const std::set<std::string> &prefixes, const std::set<char> &alphabet, const std::chrono::seconds &timeLimit, long double *timeTaken) {
        std::atomic_bool stopTrigger(false);
        bool stopBool(false);

        std::chrono::seconds remainingTime(timeLimit);

        std::packaged_task<std::unique_ptr<Method>()> task([&]() -> std::unique_ptr<Method> {
            std::unique_ptr<Method> bestPossible;

            unsigned int nMin = 1, nMax = prefixes.size();

            // We know that the prefix acceptor is consistent (it might be the smallest DFA)
            bestPossible = constructMethodTrigger(method, nMax, Sp, Sm, S, prefixes, alphabet, timeLimit, stopTrigger, &stopBool);
            bestPossible->solve();
            if (timeTaken) {
                *timeTaken = bestPossible->timeToSolve();
            }
            if (!bestPossible->hasSolution()) {
                return nullptr;
            }

            unsigned int n = std::ceil(nMin + (nMax - nMin) / 2.);

            while (n != nMax && n != nMin && !stopTrigger) { // In case a method manages to escape the trigger, we force it to stop here
                // std::cout << methodsNames.at(method) << " " << n << "\n";
                std::unique_ptr<Method> toTry = constructMethodTrigger(method, n, Sp, Sm, S, prefixes, alphabet, timeLimit, stopTrigger, &stopBool);

                bool success = toTry->solve();
                if (timeTaken) {
                    *timeTaken += toTry->timeToSolve();
                }
                remainingTime -= std::chrono::seconds(int(std::ceil(toTry->timeToSolve())));

                if (success) {
                    if (toTry->numberOfStates() < bestPossible->numberOfStates()) {
                        bestPossible = std::move(toTry);
                    }
                    nMax = n;
                }
                else {
                    nMin = n;
                }

                n = std::ceil(nMin + (nMax - nMin) / 2.);
            }
            return bestPossible;
        });

        std::future<std::unique_ptr<Method>> future = task.get_future();
        std::thread t(std::move(task));

        // If there is no limit
        if (timeLimit <= std::chrono::seconds(0)) {
            // std::cout << methodsNames.at(method) << " NO LIMIT\n";
            future.wait();
            t.join();
            return std::make_pair(future.get(), true);
        }
        else {
            // std::cout << methodsNames.at(method) << " LIMIT\n";
            std::future_status status = future.wait_for(timeLimit);
            if (status == std::future_status::ready) {
                // std::cout << methodsNames.at(method) << " ready\n";
                t.join();
                return std::make_pair(future.get(), true);
            }
            else {
                // std::cout << methodsNames.at(method) << " timeout\n";
                *timeTaken = timeLimit.count();
                stopTrigger = true;
                stopBool = true;
                t.join();
                return std::make_pair(nullptr, false);
            }
        }
    }

    Method::Method(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n) :
        m_Sp(SpSet),
        m_Sm(SmSet),
        m_numberStates(n),
        m_S(SSet),
        m_prefixes(prefixesSet),
        m_alphabet(alphabetSet),
        m_triedSolve(false)
    {

    }

    Method::~Method() {
    }

    unsigned int Method::numberOfStates() const {
        return m_numberStates;
    }

    double Method::timeToSolve() const {
        return m_cpuTimeEnd - m_cpuTimeStart;
    }
}
