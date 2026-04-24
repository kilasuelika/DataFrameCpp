# AGENTS.md

This file is the working guide for humans and coding agents operating in the active `DataFrameCpp` codebase.

## Scope

- Treat `old_version/` as out of scope. It is legacy code and should not be used for architecture research, edits, or documentation.
- Focus on the active implementation under `include/DataFrameCpp/`, `test/`, `vs_tests/`, and `README.md`.

## Project Snapshot

`DataFrameCpp` is a header-only C++ dataframe library with a pandas-like API. The active codebase is organized around a small set of core runtime types:

- `Series`: a dynamically typed column backed by a `std::variant` of concrete `std::vector<T>` storage.
- `Index`: the row-addressing layer used by both concrete dataframes and views.
- `DataFrame`: an owning table made of multiple `Series` plus a shared index.
- `SeriesView` and `DataFrameView`: non-owning projection types used for slicing and in-place updates.
- `IO`: CSV import and export helpers.

The umbrella include is `include/DataFrameCpp/DataFrameCpp.hpp`, which pulls in the implementation headers for the header-only build.

## Read Order

When you need to understand or modify behavior, read files in this order:

1. `README.md`
2. `include/DataFrameCpp/typedef.hpp`
3. `include/DataFrameCpp/Series.hpp`
4. `include/DataFrameCpp/Index.hpp`
5. `include/DataFrameCpp/DataFrame.hpp`
6. `include/DataFrameCpp/DataFrameView.hpp`
7. Matching files under `include/DataFrameCpp/impl/`
8. Tests under `test/`

## Repository Layout

- `include/DataFrameCpp/`: public declarations and umbrella include.
- `include/DataFrameCpp/impl/`: inline implementations for the header-only library.
- `include/DataFrameCpp/macro/`: macro helpers used by dataframe and dataframe-view arithmetic/assignment code.
- `test/`: Boost.Test test cases and CSV fixtures.
- `vs_tests/`: Visual Studio solution and project for running the tests.
- `README.md`: project overview, examples, and high-level reference notes.

More detail lives in:

- `docs/architecture.md`
- `docs/repo-map.md`
- `docs/testing.md`

## Core Architecture

### Type System

`typedef.hpp` defines the runtime type model:

- `DType`: logical scalar types such as `STRING`, `INT`, `DOUBLE`, `DATE`, and `DATETIME`.
- `SeriesType`: a `std::variant` of supported `std::vector<T>` column containers.
- Concepts like `supported_type`, `iloc_type`, and `supported_functor_type` gate template APIs.

### Ownership Model

- `DataFrame` owns heap-allocated `Series` instances in `_values`.
- `DataFrameView` stores heap-allocated `SeriesView` wrappers and a shared or derived `Index`.
- View operations are intentionally lightweight and often mutate the underlying owning dataframe through shared storage.

### Indexing Model

- The default row index is `Int64RangeIndex`.
- `Index` also supports key-based subclasses such as `KeyValueIndex<long long>`.
- Row selections are implemented by building derived positional maps rather than copying every column immediately.

### Header-Only Structure

- Public declarations live in `*.hpp` files at `include/DataFrameCpp/`.
- Inline definitions live in `include/DataFrameCpp/impl/*.hpp`.
- If you add a new public API, keep the declaration and inline implementation in sync and ensure it is reachable from `DataFrameCpp.hpp`.

## Working Rules

- Prefer modifying the active tree only. Do not port behavior from `old_version/` without an explicit request.
- Check both declaration and implementation files before changing any public API.
- Preserve the current header-only style: most behavior is defined inline in the `impl/` headers.
- Use the existing tests as the source of truth for public usage patterns.
- Keep examples and docs aligned with the current implementation, not the roadmap alone.

## High-Risk Areas

- View semantics: `DataFrameView` and `SeriesView` are non-owning and can reflect mutations back into the source dataframe.
- Duplicate column names: the code uses `std::multimap<std::string, size_t>` for column lookup, so name collisions are allowed and affect selection behavior.
- Index evolution: `set_index()` and `create_index()` are only partially realized in the active tree, so index-related changes need extra care.
- Header coupling: a change in one inline implementation can affect many translation units because the library is header-only.
- Mixed typed arithmetic and `apply()`: behavior depends on runtime dtype matching and template functor traits.

## Dependencies And Tooling

- Language level: modern C++ with C++20 features, and the Visual Studio project currently uses `stdcpplatest`.
- External libraries used in the active tree: Boost, Eigen, and `fmt`.
- Test framework: Boost.Test.
- Local test harness: `vs_tests/vs_tests.sln`.

## Change Checklist

Before finishing a change in this repository:

1. Confirm the change is in the active tree, not `old_version/`.
2. Update both declaration and inline implementation if a public symbol changes.
3. Review affected tests under `test/`.
4. Add or adjust documentation when behavior, architecture, or contributor guidance changes.
5. Watch for view lifetime and index behavior regressions.

## Known Gaps In The Active Tree

These are worth understanding before making broad changes:

- Some README roadmap items are not implemented yet.
- Base `Index::loc()` overloads still throw `std::logic_error("Not implemented.")`.
- Parts of index creation and non-inplace `set_index()` are stubbed or incomplete.
- The active code still contains a few debug-style prints and rough edges that should be treated carefully during cleanup.

## Documentation Intent

This doc is meant to keep future edits oriented around the real codebase structure. If the architecture changes, update this file first, then update the supporting docs in `docs/`.
