# Testing

This document describes the current testing setup for the active `DataFrameCpp` tree. It ignores `old_version/`.

## Current Test Stack

- Framework: Boost.Test
- Harness entry point: `vs_tests/vs_tests/vs_tests.cpp`
- Main local project: `vs_tests/vs_tests.sln`
- Test sources: `test/*.cpp`

The repository currently ships a Visual Studio solution rather than a cross-platform build file such as CMake.

## Test Inventory

### `test/test_Series.cpp`

Covers basic `Series` construction and positional indexing.

### `test/test_DataFrame.cpp`

Covers dataframe construction and basic dataframe arithmetic.

### `test/test_DataFrameView.cpp`

Covers:

- `iloc()` slicing
- view arithmetic
- assignment through views
- `append_row().set(...)` usage

### `test/test_apply.cpp`

Covers unary `apply()` and template-functor `apply()` on dataframes.

### `test/test_index.cpp`

Contains early index-oriented checks, though current coverage here is still thin.

## Running Tests

The repository already includes a Visual Studio project that references all active tests and headers:

- Open `vs_tests/vs_tests.sln`
- Build the `vs_tests` project
- Run the resulting Boost.Test executable

The project file currently pulls in:

- all test files from `test/`
- headers from `include/DataFrameCpp/`
- `README.md` as a non-code project item

## How To Add Tests

When adding or changing behavior:

1. Put new test cases in the closest existing `test/*.cpp` file when the behavior matches that file's focus.
2. Create a new `test_*.cpp` file only when the new behavior introduces a distinct area that would otherwise make existing files noisy.
3. If you add a new test file, also add it to `vs_tests/vs_tests/vs_tests.vcxproj`.

## What To Prioritize

High-value test targets in the current codebase are:

- view lifetime and copy boundaries
- duplicate column-name behavior
- index creation and `loc()` semantics
- dtype conversion and `apply()` dispatch
- CSV type inference edge cases

## Current Gaps

The active tests are useful but still lightweight. They do not comprehensively lock down:

- incomplete index paths
- all failure modes in CSV import
- shape and duplicate-name edge cases
- interactions between views and later dataframe mutation

## Documentation Rule

If you change public behavior and add or update tests, update `AGENTS.md` or the matching doc in `docs/` when the change affects contributor expectations.
