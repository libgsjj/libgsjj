/**
 * \file binary.h
 * 
 * This file declares the function to convert a number in its decimal form to its binary form and vice-versa.
 */

#pragma once

#include <vector>

namespace gsjj {
    /**
     * Converts a number in decimal form to binary form.
     * 
     * The binary form is stored in a vector of boolean values (just like a number written in binary).
     * @param dec_number The number in its decimal form
     * @param size The size of the binary form (\f$m\f$)
     * @return The number in its binary form
     */
    std::vector<bool> decimal_to_binary(unsigned int dec_number, unsigned int size);
    /**
     * Converts a number in binary form to decimal form.
     * @param binary_number The number in its binary form
     * @return The number in its decimal form
     */
    unsigned int binary_to_decimal(const std::vector<bool> &binary_number);
}