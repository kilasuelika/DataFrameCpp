#pragma once

// @kilasuelika
// May 19, 2022

#include <boost/test/unit_test.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>
#include "../include/DataFrameCpp/DataFrameCpp.hpp"

BOOST_AUTO_TEST_SUITE(test_DataFrameView)

BOOST_AUTO_TEST_CASE(series_uniform_initialization) {
    dfc::DataFrame df{{"a", {1.0, 2.0, 9.0, 5.0}}, {"b", {6, 7, 8, 9}}};
    // BOOST_CHECK( == 4);
}

BOOST_AUTO_TEST_CASE(base_arithmetic) {
    dfc::DataFrame df{
        {"a", {1.0, 2.0, 9.0}}, {"b", {6, 8, 9}}, {"c", {4, 5, 6}}, {"d", {1.2, 9.7, 8.6}}};
    dfc::DataFrame df1{{"c", {4, 5, 6}}, {"d", {1.2, 9.7, 8.6}}};
    auto df2 = df.iloc({2, 1}, {"d", "a"});
    BOOST_CHECK_CLOSE(df2.iloc<double>(0, 0), 8.6, 1e-10);

    auto df3 = df.iloc({1, 2}, {"c"});
    BOOST_CHECK(df3.iloc<int>(0, 0)==5);
    BOOST_CHECK(df3.iloc<int>(-1, 0)==6);

    auto df4 = df.iloc({1, 2}, {"b"});
    df3 = df4;
    BOOST_CHECK(df.iloc<int>(0,2)==4);
    BOOST_CHECK_EQUAL(df.iloc<int>(1, 2), 8);
}

BOOST_AUTO_TEST_SUITE_END()
