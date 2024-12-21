# DataFrameCpp - A C++ Dynamic DataFrame library with Python Pandas-like API inspired by hosseinmoein/DataFrame

## Goal and Limitation

The goal of this library is to provide a dynamic DataFrame library for C++ with pandas-like API. Currently it mainly deal with data management. [hosseinmoein/DataFrame](https://github.com/hosseinmoein/DataFrame) is a good data library. But I feel upset when I use it due to it is mainly designed for static column types and lacking data manuplating methods, e.g. remove rows and cast column type. So I decide to write my own dataframe library. 

"Dynamic" means data type information will be stored (I use `variant` to store data) and you don't need to specify data types when programming at most time. What's more, the column type can be changed on-the-fly. Of course, the trade-off is **Lower Performace** as type information must be matched at runtime. However the user can also use function of template version and provide a known data type to avoid runtime comparison.

If performance is critical, I suggest you to try [hosseinmoein/DataFrame](https://github.com/hosseinmoein/DataFrame) which is much more mature.

Other limitations are:

1. You need a C++ 20 compilier.
2. It depends on some external libaries: `Eigen`, `boost`.
3. If you want to do more computation, you need to learn `Eigen` or `Armadillo`. This library provides some API to convert data types. It's up to you on dealing with it.

This library try to provide pandas-like API, however there are some important differences except pandas is way more complete:

1. Subscripts usually return a view reffers to the original dataframe. Different rows in a view may refer to the same element.

If you remove rows from the original dataframe, then you shouldn't use views created form the dataframe any more.

2. Most modifications happen **in place** and there is no `in_place` option like pandas. In my own experience with pandas, I find it's really a pain to set `in_place = true` every time.

3. You can't use subscripts to create a new column. Because I use vector of primitive data types. So there is no direct way to represent `None`.

## Task

- [x] `apply`
- [ ] `select()`
- [ ] `groupby()`
- [ ] `sort()`
- [ ] Key-value Index
- [ ] Statistical and math functions
- [ ] Dump to binary and load
- [ ] Hierarchical index.
- [ ] `DATE` and `DATETIME` data types using `boost.datetime`

## Installation

It's a header-only library.

## Examples

### Hellow World
```cpp

#include "DataFrameCpp/include/DataFrameCpp/DataFrameCpp.hpp"

int main() {
    dfc::DataFrame df{{"a", {1.0, 2.0, 9.0}},
                      {"b", {6, 8, 9}},
                      {"c", {4, 5, 6}},
                      {"d", {1.2, 9.7, 8.6}},
                      {"e", {"A", "B", "C"}}};

    std::cout << df << std::endl;
}
```

Output:

```
           [0]       [1]       [2]       [3]       [4]
             a         b         c         d         e
        double       int       int    double    string
   0         1         6         4       1.2         A
   1         2         8         5       9.7         B
   2         9         9         6       8.6         C
Index:  (trival)
Shape: (3, 5)
```

### A More Complated Example


## Usages

### Availiable Column Data Types

```cpp
enum DType { NONE = 0, STRING, BOOL, INT, LONGLONG, FLOAT, DOUBLE, DATE, DATETIME, DATEDURATION, TIMEDURATION };
```

### Read from CSV Files

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

```cpp
dfc::DataFrame df{{"a", {1.0, 2.0, 9.0}},
                      {"b", {6, 8, 9}},
                      {"c", {4, 5, 6}},
                      {"d", {1.2, 9.7, 8.6}},
                      {"e", {"A", "B", "C"}}};
```
### Create an empty DataFrame and set data later

```cpp
dfc::DataFrame df({"a","b","c"},{DType::STRING,DType::DOUBLE, DType::INT});  //3 colums.

// append column
df.append_col("f", dfc::DType::STRING); //blank column
df.append_col("g", {"11", "12", "13"});

// append row
df.append_row(); // blank row. return a DataFrameView of the last row.
df.append_row().set("a", 5).set("b", 7);  // Append a row and set value. 
```

### Subscription

#### By integer position

```cpp
DataFrameView df1 = df.iloc({2, 1}, {"d", "a"});  //column d and a, row 2 and 1.
```

#### By index

```cpp
DataFrameView 2=df.set_index("b");

df.loc({8, 9}, {"d","a"});
```

### Computation

#### Math functions

#### Apply

##### Pass a concrete unary function

When you pass a concrete unary function, only columns have the same data type will be applied by the function and other columns won't change.
```cpp
dfc::DataFrame df{{"a", {1.0, 2.0, 9.0, 8.0}}, {"b", {6, 7, 8, 9}}};

std::function<double(double)> double_plus_1_functor = [](double x) { return x + 1; };
df.apply<true>(double_plus_1_functor); // values of column a changed and b no change. `true` for inplace.
df.apply_in_place(double_plus_1_functor);  // the same above.

auto double_to_int_functor = [](double x) { return int(x); };
df.apply<true>(double_to_int_functor); // Now a is a int column.

df.apply<true>([](int x) { return x + 2; }); // pass lambda directly.
```

##### Pass a template functor with unique return type

Consider a `to_string()` that can accept multiple type inputs and a known unique return type. Then you can pass a template functor as a template argument and specify a list of target argument types to convert specific type of columns to string.

```cpp
template <typename T> struct to_string_functor {
    std::string operator()(T x) { return std::to_string(x); }
};

dfc::DataFrame df{{"a", {1.0, 2.0, 9.0, 8.0}}, {"b", {6, 7, 8, 9}}, {"c", "a", "bc", "d", "e"}};
df.apply<true, to_string_functor, double, int>();   //You only want to convert double and int columns to string.
```

#### Group processing



## Reference

### Core data structure

#### Series

Type is `vector<T>`. It's always contiguous.

#### Index

Use a `Series` to store keys and use `unordered_map` to store key-index map. The key map is a variant to represent range(contiguous integer indexing), int and string indexing.

If the key map is long long, then the value is the start of indexing.

Type of indices:

1. `template <typename KeyType> class KeyValueIndex`: key-value type index. Multiple rows can associated with a single key.
2. `template <typename KeyType> class UniqueKeyValueIndex`: Unique version key-value type index. An exception will be generated when there are multiple rows for a single key.
1. 
#### DataFrame

Consists of `Index` and multiple `Series`.

#### ViewIndex

Use `Index*` to point to a basis index. The `ViewIndex` **may not** be contigous. So there is a pos `vector<size_t>` to store index-positon map. For example, `[ 5, 0, 1, 2, 4]` means the first row is the 5th element in `Index`, the second row is the 0-th element in `Index`.

This is usefull for sorting and subscripting. For example, we can use a view to record the order of sorting results without actually reorder all rows(which requires copying and moving data between rows).

The `ViewIndex` can running on `Index* = nullptr`. Then it can only use `pos` to generate index.

Note that `ViewIndex` is the index of view, not view of index.

In-contigouos data may slow computation performance thus you should try `copy` to create a new concrete `DataFrame` and do computation on it.

`ViewIndex` has a key map that map integer location to the location in original index.

#### SeriesView

Contains a pointer to `Series` and a `ViewIndex`.

#### DataFrameView

Consists of `vector<SeriesView*>`. This is the most complex data structure. Each column are independent thus complications rise when concatenation:

1. Different column in a `DataFrameView`  got by horizontal concatenation may refer to the same basis Series.

2. Different elements in one column may refer to the same element by vertical concatenation. To reduce complexity, vertical concatenation always return a concrete `DataFrame` (thus must copy data).

3. If the `DataFrameView` has multiple columns that have the same name. Then the `column_map` only store the last index.