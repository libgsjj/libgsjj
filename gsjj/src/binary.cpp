/**
 * \file binary.cpp
 * 
 * This file implements the functions to convert a number in its decimal form to its binary form and vice-versa.
 * 
 * See binary.h
 */

#include "binary.h"

namespace gsjj {
    std::vector<bool> decimal_to_binary(unsigned int dec_number, unsigned int size) {
        if (dec_number == 0) {
            return std::vector<bool>(size, 0);
        }
        unsigned int tmp = dec_number;
        std::vector<bool> res;
        res.reserve(size);
        while (tmp > 0) {
            if (tmp % 2 == 0) {
                res.push_back(0);
                tmp = tmp / 2;
            }
            else {
                res.push_back(1);
                tmp = tmp - 1;
                tmp = tmp / 2;
            }
        }

        while (res.size() < size) {
            res.push_back(0);
        }
        return res;
    }

    unsigned int binary_to_decimal(const std::vector<bool> &binary_number) {
        unsigned int res = 0;
        unsigned int actual_exp = 1;
        for (unsigned int i = 0 ; i < binary_number.size() ; i++) {
            unsigned int bit = binary_number[i];
            res += actual_exp * bit;
            actual_exp *= 2;
        }
        return res;
    }
}