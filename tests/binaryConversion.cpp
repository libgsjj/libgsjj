#include "catch.hpp"

#include "binary.h"

TEST_CASE("Binary to decimal", "[conversions]") {
    SECTION("Conversion of 6 to 011") {
        auto bin = decimal_to_binary(6, 3);
        REQUIRE(bin == std::vector<bool>({0, 1, 1}));
    }
    SECTION("Conversion of 0 to 0") {
        auto bin = decimal_to_binary(0, 1);
        REQUIRE(bin == std::vector<bool>({0}));
    }
    SECTION("Conversion of 1 to 1") {
        auto bin = decimal_to_binary(1, 1);
        REQUIRE(bin == std::vector<bool>({1}));
    }
}

TEST_CASE("Decimal to binary", "[conversions]") {
    SECTION("Conversion of 011 to 6") {
        unsigned int dec = binary_to_decimal({0, 1, 1});
        REQUIRE(dec == 6);
    }
    SECTION("Conversion of 0 to 0") {
        unsigned int dec = binary_to_decimal({0});
        REQUIRE(dec == 0);
    }
    SECTION("Conversion of 1 to 1") {
        unsigned int dec = binary_to_decimal({1});
        REQUIRE(dec == 1);
    }
}