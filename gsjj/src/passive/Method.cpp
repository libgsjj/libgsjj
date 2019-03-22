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

namespace gsjj {
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

            std::unique_ptr<Method> bestPossible = nullptr;

            // We use a thread to be able to stop the method when the time limit is reached
            std::packaged_task<void()> task([&]() {
                unsigned int nMin = 1, nMax = prefixes.size();

                // We know that the prefix acceptor is consistent (it might be the smallest DFA)
                bestPossible = constructMethodTrigger(method, nMax, Sp, Sm, S, prefixes, alphabet, timeLimit, stopTrigger, &stopBool);
                bestPossible->solve();
                if (timeTaken) {
                    *timeTaken = bestPossible->timeToSolve();
                }
                if (!bestPossible->hasSolution()) {
                    return;
                }
                remainingTime -= std::chrono::seconds(int(std::floor(bestPossible->timeToSolve())));

                unsigned int n = std::ceil(nMin + (nMax - nMin) / 2.);

                while (n != nMax && n != nMin && !stopTrigger && remainingTime != std::chrono::seconds(0)) { // In case a method manages to escape the trigger, we force it to stop here
                    // std::cout << methodsNames.at(method) << " " << n << "\n";
                    std::unique_ptr<Method> toTry = constructMethodTrigger(method, n, Sp, Sm, S, prefixes, alphabet, remainingTime, stopTrigger, &stopBool);

                    bool success = toTry->solve();
                    if (timeTaken) {
                        *timeTaken += toTry->timeToSolve();
                    }
                    remainingTime -= std::chrono::seconds(int(std::floor(toTry->timeToSolve())));

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
            });

            std::future<void> future = task.get_future();
            std::thread t(std::move(task));

            // If there is no limit, we just start the thread and join
            if (timeLimit <= std::chrono::seconds(0)) {
                future.wait();
                t.join();
                return std::make_pair(std::move(bestPossible), true);
            }
            else {
                // We use wait_for to wait for the thread to finish or for the timeLimit to be reached
                // If the thread had enough time, we simply join (to make sure everything is ready to be destroyed) and we return the built model
                // If the time limit is reached, we set the stopping flags to ask the thread to finish as soon as possible and we join
                std::future_status status = future.wait_for(timeLimit);
                if (status == std::future_status::ready) {
                    t.join();
                    return std::make_pair(std::move(bestPossible), true);
                }
                else {
                    stopTrigger = true;
                    stopBool = true;
                    t.join();
                    return std::make_pair(std::move(bestPossible), false);
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
            m_triedSolve(false),
            m_cpuTimeStart(0),
            m_cpuTimeEnd(0)
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
}