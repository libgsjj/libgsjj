#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <future>
#include <cmath>

#include "gsjj/passive/MethodFactory.h"
#include "gsjj/passive/utils.h"

std::atomic_bool falseStopTrigger(false);
std::chrono::seconds noLimit(0);
bool falseStopBool;

namespace gsjj {
    namespace passive {
        // We initialise the static map
        std::map<std::string, MethodFactory::MethodConstructor> MethodFactory::s_methodsMap;

        bool MethodFactory::registerMethod(const std::string &name, MethodConstructor const &constructor) {
            // If "name" is not yet registered, we register it (it's really simple)
            auto itr = s_methodsMap.find(name);
            if (itr == s_methodsMap.end()) {
                s_methodsMap.emplace(name, constructor);
                return true;
            }
            else {
                return false;
            }
        }

        MethodFactory::MethodPtr MethodFactory::create(const std::string &name, unsigned int n, const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::set<std::string> &S, const std::set<std::string> &prefixes, const std::set<char> &alphabet) {
            auto itr = s_methodsMap.find(name);
            if (itr == s_methodsMap.end()) {
                return nullptr;
            }
            else {
                return itr->second(Sp, Sm, S, prefixes, alphabet, n);
            }
        }

        std::unique_ptr<Method> constructMethodTrigger(const std::string &name, unsigned int n, const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::set<std::string> &S, const std::set<std::string> &prefixes, const std::set<char> &alphabet, const std::chrono::seconds &timeLimit, std::atomic_bool &stopTrigger, const bool *stopPointer) {
            std::unique_ptr<Method> method = MethodFactory::create(name, n, Sp, Sm, S, prefixes, alphabet);
            if (!method) {
                throw std::invalid_argument("Unknown method type: " + name);
            }
            method->setStopTrigger(timeLimit, stopTrigger, stopPointer);
            return method;
        }

        std::unique_ptr<Method> constructMethod(const std::string &method, unsigned int n, const std::set<std::string> &Sp, const std::set<std::string> &Sm) {
            std::set<std::string> S = computeS(Sp, Sm);
            std::set<std::string> prefixes = computePrefixes(S);
            std::set<char> alphabet = computeAlphabet(S);
            
            return std::move(constructMethod(method, n, Sp, Sm, S, prefixes, alphabet));
        }

        std::unique_ptr<Method> constructMethod(const std::string &method, unsigned int n, const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::set<std::string> &S, const std::set<std::string> &prefixes, const std::set<char> &alphabet) {
            return std::move(constructMethodTrigger(method, n, Sp, Sm, S, prefixes, alphabet, noLimit, falseStopTrigger, &falseStopBool));
        }

        std::pair<std::unique_ptr<Method>, bool> constructMethod(const std::string &method, const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::chrono::seconds &timeLimit, long double *timeTaken) {
            std::set<std::string> S = computeS(Sp, Sm);
            std::set<std::string> prefixes = computePrefixes(S);
            std::set<char> alphabet = computeAlphabet(S);
            
            return constructMethod(method, Sp, Sm, S, prefixes, alphabet, timeLimit, timeTaken);
        }

        std::pair<std::unique_ptr<Method>, bool> constructMethod(const std::string &method, const std::set<std::string> &Sp, const std::set<std::string> &Sm, const std::set<std::string> &S, const std::set<std::string> &prefixes, const std::set<char> &alphabet, const std::chrono::seconds &timeLimit, long double *timeTaken) {
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

                // We seek the best possible DFA size using a binary search
                // We stop if we reach the end of a classic binary search but also when the stop is triggered
                while (n != nMax && n != nMin && !stopTrigger) {
                    std::unique_ptr<Method> toTry = constructMethodTrigger(method, n, Sp, Sm, S, prefixes, alphabet, remainingTime, stopTrigger, &stopBool);

                    bool success = toTry->solve();
                    if (timeTaken) {
                        *timeTaken += toTry->timeToSolve();
                    }
                    remainingTime -= std::chrono::seconds(int(std::floor(toTry->timeToSolve())));
                    // To make sure CVC4 does not think it has an unlimited available time
                    if (remainingTime.count() <= 0) {
                        if (timeLimit == std::chrono::seconds(0)) {
                            remainingTime = std::chrono::seconds(0);
                        }
                        else {
                            remainingTime = std::chrono::seconds(1);
                        }
                    }

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
                future.get(); // Allows to retrieve any thrown exception
                return std::make_pair(std::move(bestPossible), true);
            }
            else {
                // We use wait_for to wait for the thread to finish or for the timeLimit to be reached
                // If the thread had enough time, we simply join (to make sure everything is ready to be destroyed) and we return the built model
                // If the time limit is reached, we set the stopping flags to ask the thread to finish as soon as possible and we join
                std::future_status status = future.wait_for(timeLimit);
                if (status == std::future_status::ready) {
                    t.join();
                    future.get(); // Allows to retrieve any thrown exception
                    return std::make_pair(std::move(bestPossible), true);
                }
                else {
                    stopTrigger.store(true);
                    stopBool = true;
                    t.join();
                    future.get(); // Allows to retrieve any thrown exception
                    return std::make_pair(std::move(bestPossible), false);
                }
            }
        }
    }
}
