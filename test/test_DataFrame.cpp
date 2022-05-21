#pragma once

// @kilasuelika
// May 19, 2022

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include "../include/DataFrameCpp/DataFrame.hpp"

BOOST_AUTO_TEST_SUITE(test_DataFrame)

BOOST_AUTO_TEST_CASE(series_uniform_initialization) {
    dfc::DataFrame<int, dfc::Series> df{{"a", {1.0, 2.0, 9.0}}, {"b", {6, 7, 8, 9}}};
    // BOOST_CHECK( == 4);
}

BOOST_AUTO_TEST_SUITE_END()