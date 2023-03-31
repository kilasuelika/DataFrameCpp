#pragma once

// @kilasuelika
// May 19, 2022

#include <boost/test/unit_test.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>

#include "../include/DataFrameCpp/impl/SeriesImpl.hpp"

BOOST_AUTO_TEST_SUITE(test_Series)

BOOST_AUTO_TEST_CASE(series_construction) {
    std::vector<double> v{1.0, 2.0, 3.0};
    dfc::Series s0(v);

    dfc::Series s1({1.0, 2.0, 6.0});
    dfc::Series s2{"a", {1, 6, 5, 1}};
}

BOOST_AUTO_TEST_CASE(series_subscripts) {
    std::vector<double> v{1.0, 2.0, 3.0};
    dfc::Series s0(v);

    dfc::Series s1({1.0, -1.0, 2.0, 6.0});
    dfc::Series s2{"a", {1, 6, 5, 1}};

    BOOST_CHECK(s0.iloc<double>(2) == 3.0);
    BOOST_CHECK(s1.iloc<double>(-1) == 6.0);
}

BOOST_AUTO_TEST_SUITE_END()
