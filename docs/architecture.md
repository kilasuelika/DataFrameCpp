# Architecture

This document describes the active architecture of `DataFrameCpp`. It intentionally ignores `old_version/`.

## Overview

The library is a header-only dataframe implementation centered on runtime-typed columns. Instead of templating the entire dataframe on column types, it stores each column in a type-erased variant and dispatches operations based on runtime dtype.

At a high level:

1. `typedef.hpp` defines the scalar type universe and compile-time concepts.
2. `Series` implements owning column storage.
3. `Index` maps user-facing row positions or keys to stored row positions.
4. `DataFrame` owns columns plus an index.
5. `SeriesView` and `DataFrameView` provide non-owning selections over existing data.
6. `IO.hpp` performs CSV import and export.

## Core Data Structures

### `DType` And `SeriesType`

The type system lives in `include/DataFrameCpp/typedef.hpp`.

- `DType` is the logical enum used by the public API.
- `DTypeMap` converts concrete C++ scalar types into `DType`.
- `SeriesType` is the concrete storage variant, currently holding vectors for strings, booleans, integers, floating-point values, and Boost date/time types.

This design allows the library to present a dataframe-like API without requiring callers to parameterize the whole table on a compile-time schema.

### `Series`

`Series` is the owning column primitive.

- Backed by `SeriesType`, which stores exactly one `std::vector<T>` alternative.
- Carries a column name and logical size.
- Exposes `iloc()` access, `astype()`, arithmetic, comparison, and `apply()` operations.
- Serves as the storage unit that `DataFrame` aggregates.

Most `Series` behavior is implemented inline in `include/DataFrameCpp/impl/SeriesImpl.hpp`.

### `Index`

`Index` is the row-addressing abstraction shared across dataframe and view objects.

- Base `Index` stores `_pos`, a positional mapping into underlying row storage.
- `Int64RangeIndex` is the default contiguous row index.
- `Int64PeriodIndex` provides stepped numeric indexing.
- `KeyValueIndex<KeyType>` supports key-to-row lookups where a key may map to multiple rows.

The current implementation is still uneven:

- Base `loc()` methods are not implemented.
- `create_index()` is only partially realized in the active tree.
- `set_index()` exists on `DataFrame`, but the non-inplace path is incomplete.

## Dataframe And View Model

### `DataFrame`

`DataFrame` is the owning table abstraction.

- Owns `Series*` in `_values`.
- Tracks row and column shape in `_shape`.
- Uses a `std::multimap<std::string, size_t>` as `_column_map`, which allows duplicate column names.
- Holds a shared index pointer in `_index`.

Important behavior:

- Column appends and inserts validate row counts.
- `append_row()` grows every column and extends the index.
- `operator[]` returns a `DataFrameView`, not a copied `Series`.
- Arithmetic between dataframes and views generally produces a concrete `DataFrame`.

### `DataFrameView`

`DataFrameView` is the main non-owning projection type.

- Stores `SeriesView*` rather than full columns.
- Carries its own `_shape`, `_column_map`, and shared or derived `_index`.
- Supports `iloc()` slicing, `set()`, assignment, arithmetic, CSV export, and copy-to-concrete via `copy()`.

The important semantic point is that a view is often a live window onto the original dataframe. Many operations mutate underlying storage through those view objects.

## Execution Model

### Row Selection

Row slicing usually creates a derived `Index` whose positional map points back into the source data. This avoids immediate full-column copying and is the basis for view semantics.

### Column Selection

Column slicing builds new `SeriesView` wrappers over selected source columns. Because the column map is a multimap, a single name may resolve to more than one logical column.

### Copy Boundary

`DataFrameView::copy()` and `DataFrame(const DataFrameView&)` convert a view into an owning concrete dataframe. This is the main escape hatch when a caller wants independence from source storage.

## CSV IO Flow

CSV import and export are intentionally simple.

### Read Path

`read_csv()` in `IO.hpp`:

1. Validates file existence.
2. Calls `guess_column_types()` to infer names and dtypes from early rows.
3. Builds a blank dataframe with those columns and dtypes.
4. Appends rows and parses tokens into the selected concrete vector type.

Current inference is conservative and mainly distinguishes `INT`, `DOUBLE`, and `STRING`.

### Write Path

`DataFrame::to_csv()` and `DataFrameView::to_csv()` write the current visible shape, optionally including header and index.

## Header-Only Layout

The codebase uses a declaration-plus-inline-implementation pattern:

- Public API declarations: `include/DataFrameCpp/*.hpp`
- Inline implementations: `include/DataFrameCpp/impl/*.hpp`
- Aggregator include: `include/DataFrameCpp/DataFrameCpp.hpp`

This means architectural changes often require edits in more than one file even when the project appears small.

## Implementation Hotspots

These areas deserve extra caution:

- `DataFrameImpl.hpp`: core mutation, indexing, arithmetic, and ownership behavior.
- `DataFrameViewImpl.hpp`: view construction, projection semantics, assignment, and CSV output.
- `SeriesImpl.hpp`: dtype conversion, arithmetic, and runtime dispatch.
- `IndexImpl.hpp`: cloning and positional mapping.
- `typedef.hpp`: any type-system change ripples through almost the whole library.

## Current Constraints

- The project is coupled to Boost, Eigen, and `fmt`.
- The library relies on modern C++ facilities such as concepts, `std::format`, and `std::variant`.
- Some features are still roadmap items rather than finished implementations.
- Tests cover core behavior but do not yet fully specify all edge cases around indexing and view lifetime.
