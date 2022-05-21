# DataFrameCpp - A C++ Dynamic DataFrame library with Python Pandas-like API inspired by hosseinmoein/DataFrame

## Goal and Limitation

The goal of this library is to provide a DataFrame library for C++ with pandas-like API. Currently it mainly deal with data management.

"Dynamic" means data information will be stored and you don't need to speficy data types at most time. Of course, the trade-off is lower performace as type information must be matched at runtime. However the user can also use function of template version to avoid runtime check.

Other limitations are:

1. You need a C++ 20 compilier.
2. It depends on some external libaries: `Eigen`, `boost.lexical_cast`. I don't like re-invent wheels.
3. If you want to do more computation, you need to learn `Eigen` or `Armadillo`. This library provides some API to convert data types. It's up to you on dealing with it.

## Installation

## Basic Example

### Read CSV files

### Manully input data

## Subscription

## Convertion
