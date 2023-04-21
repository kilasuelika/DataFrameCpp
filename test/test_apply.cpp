#pragma once

// @kilasuelika
// Jul 2, 2023

#include <boost/test/unit_test.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>
#include "../include/DataFrameCpp/DataFrameCpp.hpp"

BOOST_AUTO_TEST_SUITE(test_apply)

BOOST_AUTO_TEST_CASE(apply_dataframe_unary_functor) {
    dfc::DataFrame df{{"a", {1.0, 2.0, 9.0, 8.0}}, {"b", {6, 7, 8, 9}}};

    std::function<double(double)> double_plus_1_functor = [](double x) { return x + 1; };
    df.apply<true>(double_plus_1_functor); // values of a changed and b no change.
    BOOST_CHECK_CLOSE(df.iloc<double>(0, 0), 2.0, 1e-10);
    BOOST_CHECK_EQUAL(df.iloc<int>(0, 1), 6);

    auto double_to_int_functor = [](double x) { return int(x); };
    df.apply<true>(double_to_int_functor); // Now a is a int column.
    BOOST_CHECK_EQUAL(df.iloc<int>(0, 0), 2);

    df.apply<true>([](int x) { return x + 2; }); // pass lambda directly.
    BOOST_CHECK_EQUAL(df.iloc<int>(0, 0), 4);
    BOOST_CHECK_EQUAL(df.iloc<int>(1, 1), 9);
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
