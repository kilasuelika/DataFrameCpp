# DataFrameCpp - A C++ Dynamic DataFrame library with Python Pandas-like API inspired by hosseinmoein/DataFrame

## Goal and Limitation

The goal of this library is to provide a DataFrame library for C++ with pandas-like API. Currently it mainly deal with data management.

"Dynamic" means data information will be stored (I use `variant` to store data) and you don't need to speficy data types at most time. Of course, the trade-off is lower performace as type information must be matched at runtime. However the user can also use function of template version to avoid runtime check.

Other limitations are:

1. You need a C++ 20 compilier.
2. It depends on some external libaries: `Eigen`, `boost.lexical_cast`, `boost.tokenizer`. I don't like re-invent wheels.
3. If you want to do more computation, you need to learn `Eigen` or `Armadillo`. This library provides some API to convert data types. It's up to you on dealing with it.

## Task

- [ ] Index
- [ ] Computation

## Installation

## Hellow world Example

```cpp
#include "DataFrameCpp/DataFrameCpp.hpp"

int main() {
    dfc::DataFrame<int, dfc::Series> df{{"a", {1.0, 2.0, 9.0}}, {"b", {6, 8, 9}}};
    std::cout << df << std::endl;
}
```

## Initializing DataFrame

### Availiable data types

```cpp
enum DType { NONE, STRING, BOOL, INT, LONGLONG, FLOAT, DOUBLE };
```

### Read From CSV Files

```cpp
auto df1 = dfc::read_csv("df1.csv");
```

### Manully input data

### Create an empty DataFrame and set data later

```cpp
DataFrame<int, Series> df({"a","b","c"},{DType::STRING,DType::DOUBLE, DType::INT});  //3 colums.
```

## Subscription

## Convertion
