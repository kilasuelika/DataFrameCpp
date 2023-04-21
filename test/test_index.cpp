#pragma once

// @kilasuelika
// Jul 2, 2023

#include <boost/test/unit_test.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>
#include "../include/DataFrameCpp/DataFrameCpp.hpp"

BOOST_AUTO_TEST_SUITE(test_key_value_index)

BOOST_AUTO_TEST_CASE(string_key_value_index) {
    dfc::DataFrame df{
        {"a", {1.0, 2.0, 9.0, 8.0}}, {"b", {6, 7, 8, 9}}, {"c", {"a", "b", "b", "a"}}};

    df.set_index("c");
}

template <typename T> struct to_string_functor {
    std::string operator()(T x) { return std::to_string(x); }
};
BOOST_AUTO_TEST_CASE(apply_dataframe_uneary_template_functor) {
    dfc::DataFrame df{{"a", {1.0, 2.0, 9.0, 8.0}}, {"b", {6, 7, 8, 9}}, {"c", "a", "bc", "d", "e"}};

    df.apply<true, to_string_functor, double, int>();

    BOOST_TEST(df.iloc<std::string>(1, 1) == "7"); // BOOST_TEST(a == c);
}

BOOST_AUTO_TEST_CASE(apply_dataframeview) {}

BOOST_AUTO_TEST_SUITE_END()
