#pragma once

// @kilasuelika
// May 19, 2022

#include <boost/test/unit_test.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>
#include "../include/DataFrameCpp/DataFrameCpp.hpp"

BOOST_AUTO_TEST_SUITE(test_DataFrame)

BOOST_AUTO_TEST_CASE(series_uniform_initialization) {
    dfc::DataFrame df{{"a", {1.0, 2.0, 9.0, 8.0}}, {"b", {6, 7, 8, 9}}};
    // BOOST_CHECK( == 4);
}

BOOST_AUTO_TEST_CASE(base_arithmetic) {
    dfc::DataFrame df{{"a", {1.0, 2.0, 9.0}}, {"b", {6, 8, 9}}};
    dfc::DataFrame df1{{"c", {4, 5, 6}}, {"d", {1.2, 9.7, 8.6}}};
    auto df2 = df + df1;

    BOOST_CHECK_CLOSE(df2.iloc<double>(2,1), 17.6, 1e-10);
    BOOST_CHECK_CLOSE(df2.iloc<double>(2,0), 15, 1e-10);
}

BOOST_AUTO_TEST_SUITE_END()
