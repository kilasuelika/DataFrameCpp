#pragma once

// @kilasuelika
// May 19, 2022

#include <boost/test/unit_test.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>
#include "../include/DataFrameCpp/DataFrameCpp.hpp"

BOOST_AUTO_TEST_SUITE(test_DataFrameView)

BOOST_AUTO_TEST_CASE(series_uniform_initialization) {
    dfc::DataFrame df{ {"a", {1.0, 2.0, 9.0, 5.0}}, {"b", {6, 7, 8, 9}} };
    // BOOST_CHECK( == 4);
}

BOOST_AUTO_TEST_CASE(dataframe_iloc) {
    dfc::DataFrame df{
        {"a", {1.0, 2.0, 9.0}}, {"b", {6, 8, 9}}, {"c", {4, 5, 6}}, {"d", {1.2, 9.7, 8.6}} };
    dfc::DataFrame df1{ {"c", {4, 5, 6}}, {"d", {1.2, 9.7, 8.6}} };
    auto df2 = df.iloc({ 2, 1 }, { "d", "a" });
    BOOST_CHECK_CLOSE(df2.iloc<double>(0, 0), 8.6, 1e-10);

    auto df3 = df.iloc({ 1, 2 }, { "c" });
    BOOST_CHECK(df3.iloc<int>(0, 0) == 5);
    BOOST_CHECK(df3.iloc<int>(-1, 0) == 6);

    auto df4 = df.iloc({ 1, 2 }, { "b" });
    df3 = df4;
    BOOST_CHECK(df.iloc<int>(0, 2) == 4);
    BOOST_CHECK_EQUAL(df.iloc<int>(1, 2), 8);
}

BOOST_AUTO_TEST_CASE(dataframeview_arithmetic) {
    dfc::DataFrame df{
        {"a", {1.0, 2.0, 9.0}}, {"b", {6, 8, 9}}, {"c", {4, 5, 6}}, {"d", {1.2, 9.7, 8.6}} };

    auto df1 = df.iloc({ 2, 1 }, { "d", "a" });
    auto df2 = df.iloc({ 1, 2 }, { "a", "b" });

    auto df3 = df1 + df2;

    BOOST_CHECK_CLOSE(df3.iloc<double>(0, 0), 8.6 + 2.0, 1e-10);
    BOOST_CHECK_CLOSE(df3.iloc<double>(1, 1), 2.0 + 9, 1e-10);
}

BOOST_AUTO_TEST_CASE(dataframeview_set) {
    dfc::DataFrame df{
        {"a", {1.0, 2.0, 9.0}}, {"b", {6, 8, 9}}, {"c", {4, 5, 6}}, {"d", {1.2, 9.7, 8.6}} };

    df.append_row().set("a", 5).set("b", 7);

    BOOST_CHECK_CLOSE(df.iloc<double>(3, 0), 5, 1e-10);
    BOOST_CHECK_EQUAL(df.iloc<int>(3, 1), 7);
}

BOOST_AUTO_TEST_SUITE_END()
