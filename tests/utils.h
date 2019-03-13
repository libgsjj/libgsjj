#pragma once

#include <memory>

#include "passive/Method.h"

void checkConsistency(std::shared_ptr<DFA<char>> dfa, const std::set<std::string> &Sp, const std::set<std::string> &Sm);