# DataFrameCpp - A C++ Dynamic DataFrame library with Python Pandas-like API inspired by hosseinmoein/DataFrame

## Goal and Limitation

The goal of this library is to provide a dynamic DataFrame library for C++ with pandas-like API. Currently it mainly deal with data management.

"Dynamic" means data type information will be stored (I use `variant` to store data) and you don't need to specify data types when programming at most time. What's more, the column type can be changed on-the-fly. Of course, the trade-off is **Lower Performace** as type information must be matched at runtime. However the user can also use function of template version and provide a known data type to avoid runtime comparison.

If performance is critical, I suggest you to try [hosseinmoein/DataFrame](https://github.com/hosseinmoein/DataFrame) which is much more mature.

Other limitations are:

1. You need a C++ 20 compilier.
2. It depends on some external libaries: `Eigen`, `boost.lexical_cast`, `boost.tokenizer`. I don't like re-invent wheels.
3. If you want to do more computation, you need to learn `Eigen` or `Armadillo`. This library provides some API to convert data types. It's up to you on dealing with it.

This library try to provide pandas-like API, however there are some important differences except pandas is way more complete:

1. Doesn't support view.

1. Most modifications happen **in place** and there is no `in_place` option like pandas. In my own experience with pandas, I find it's really a pain to set `in_place = true` every time.

2. Subscripts usually return a view and you can't add or delete elements from a view. But you can use `copy` to get a new dataframe.

## Task

- [ ] View
- [ ] Index
- [ ] Data assignment
- [ ] Arithmetic
- [ ] Dump to binary and load

## Installation

It's a header-only library.

## Hellow world Example

```cpp
#include "DataFrameCpp/DataFrameCpp.hpp"

int main() {
    dfc::DataFrame<int, dfc::Series> df{{"a", {1.0, 2.0, 9.0}}, {"b", {6, 8, 9}}};
    std::cout << df.iloc<double>(0, 0) << std::endl;
    std::cout << df << std::endl;
}
```

Output:

```
         a         b
    double       int
         1         6
         2         8
         9         9
Shape: (3, 2)
```

## Initializing DataFrame

### Availiable data types

```cpp
enum DType { NONE, STRING, BOOL, INT, LONGLONG, FLOAT, DOUBLE };
```

### Read From CSV Files

Given a `df1.csv` file:

```
a,b,c
1,6,"X2"
2,8,"X3"
9,9,"X1"
```

Use following code to read it:

```cpp
auto df1 = dfc::read_csv("df1.csv");
```

The data type will be automatically decided. However it's only for `double` and `string` so that the column type will be either `double` or `string`.

### Manully input data

### Create an empty DataFrame and set data later

```cpp
DataFrame<int, Series> df({"a","b","c"},{DType::STRING,DType::DOUBLE, DType::INT});  //3 colums.
```

## Subscription

### By integer position

### By index

## Convertion

## Computation

### Basic arithmetic operation

## Reference

### Core data structure

#### Series

Type is `vector<T>`. It's always contiguous.

#### Index

Use a `Series` to store keys and use `unordered_map` to store key-index map.

#### DataFrame

Consists of `Index` and multiple `Series`.

#### ViewIndex

Use `Index*` to point to a basis index. The `ViewIndex` **may not** be contigous. So there is a pos `vector<size_t>` to store index-positon map. For example, `[ 5, 0, 1, 2, 4]` means the first row is the 5th element in `Index`, the second row is the 0-th element in `Index`.

This is usefull for sorting and subscripting. For example, we can use a view to record the order of sorting results without actually reorder all rows(which requires copying and moving data between rows).

The `ViewIndex` can running on `Index* = nullptr`. Then it can only use pos to generate index.

Note that `ViewIndex` is the index of view, not view of index.

In-contigouos data may slow computation performance thus you should try `copy` to create a new concrete `DataFrame` and do computation on it.

#### SeriesView

Contains a pointer to `Series` and a `ViewIndex`.

#### DataFrameView

Consists of `vector<SeriesView*>`. This is the most complex data structure. Each column are independent thus complications rise when concatenation:

1. Different column in a `DataFrameView`  got by horizontal concatenation may refer to the same basis Series.

2. Different elements in one column may refer to the same element by vertical concatenation. To reduce complexity, vertical concatenation always return a concrete `DataFrame` (thus must copy data).
