# Repo Map

This map covers the active `DataFrameCpp` repository and ignores `old_version/`.

## Top-Level Layout

### `include/DataFrameCpp/`

Primary library source. The active code is split between public declarations and inline implementation headers.

- `DataFrameCpp.hpp`: umbrella include for consumers of the header-only library.
- `typedef.hpp`: runtime dtype model, concepts, and shared utility types.
- `Series.hpp`: owning column abstraction.
- `SeriesView.hpp`: non-owning column view abstraction.
- `Index.hpp`: row indexing primitives.
- `DataFrame.hpp`: owning table abstraction.
- `DataFrameView.hpp`: non-owning table projection abstraction.
- `IO.hpp`: CSV read and write helpers.
- `math.hpp`, `date.hpp`, `util.hpp`: supporting utilities and math/date helpers.

### `include/DataFrameCpp/impl/`

Inline implementation bodies for the public headers.

- `SeriesImpl.hpp`
- `SeriesViewImpl.hpp`
- `IndexImpl.hpp`
- `DataFrameImpl.hpp`
- `DataFrameViewImpl.hpp`
- `AssignmentMacro.hpp`
- `UndefMacro.hpp`

If behavior appears declared but not implemented in the public header, check this directory next.

### `include/DataFrameCpp/macro/`

Macro fragments used to generate repeated arithmetic and assignment operator code shared across dataframe and view types.

### `test/`

Boost.Test cases and test fixtures.

- `test_Series.cpp`: basic `Series` construction and indexing behavior.
- `test_DataFrame.cpp`: dataframe initialization and arithmetic.
- `test_DataFrameView.cpp`: slicing, view assignment, arithmetic, and row append usage.
- `test_apply.cpp`: unary and template-functor `apply()` behavior.
- `test_index.cpp`: early index-related coverage.
- `data/`: CSV fixtures for IO-oriented tests and experimentation.

### `vs_tests/`

Visual Studio harness for local development and test execution.

- `vs_tests.sln`: solution file.
- `vs_tests/vs_tests.vcxproj`: project file listing the active tests and headers.
- `vs_tests/vs_tests.cpp`: Boost.Test entry point.

### Root Files

- `README.md`: overview, limitations, examples, and roadmap.
- `LICENSE`: license text.
- `.gitignore`, `.gitattributes`: repo housekeeping.

## Entry Points By Task

### Understand Public API

Start with:

1. `README.md`
2. `include/DataFrameCpp/DataFrameCpp.hpp`
3. `include/DataFrameCpp/DataFrame.hpp`
4. `include/DataFrameCpp/DataFrameView.hpp`

### Debug A Column-Type Issue

Read:

1. `include/DataFrameCpp/typedef.hpp`
2. `include/DataFrameCpp/Series.hpp`
3. `include/DataFrameCpp/impl/SeriesImpl.hpp`

### Debug A Slicing Or View Issue

Read:

1. `include/DataFrameCpp/Index.hpp`
2. `include/DataFrameCpp/SeriesView.hpp`
3. `include/DataFrameCpp/DataFrameView.hpp`
4. `include/DataFrameCpp/impl/DataFrameViewImpl.hpp`

### Debug CSV Behavior

Read:

1. `include/DataFrameCpp/IO.hpp`
2. `test/data/`
3. Any nearby tests that exercise import or export paths

## Practical Notes

- The codebase is small but tightly coupled because it is header-only.
- The declaration file alone is rarely enough; most real behavior is in the matching `impl/` header.
- `old_version/` contains a prior snapshot and should not be used as a reference for active behavior unless someone explicitly asks for a legacy comparison.
