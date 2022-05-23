#pragma once

// @kilasuelika
// May 19, 2022

#include "Index.hpp"

namespace dfc {

class DataFrame;

// DataFrameView stores multiple pointers to many SeriesView.
class DataFrameView {
  public:
    DataFrameView(DataFrame &dataframe);
    // rows can contains negative index.
    DataFrameView(DataFrame &dataframe, const std::vector<long long> &rows,
                  const std::vector<std::string> &col);

    // Subscripts
    template <typename T> T &iloc(long long i, long long j);
    template <typename T> T &iloc_(size_t i, size_t j);

    std::string iloc_str(long long i, long long j) const;
    std::string iloc_str_(size_t i, size_t j) const;
    // IO
    friend std::ostream &operator<<(std::ostream &os, const DataFrameView &dv);

    // Assignment
    DataFrameView &operator=(const DataFrameView &A);

    ~DataFrameView();

  private:
    // DataFrame *_values;
    ViewIndex _index;
    std::vector<SeriesView *> _values;

    DataFrameShape _shape;
    std::unordered_map<std::string, size_t> _column_map;

    DataFrameShape _cal_index(long long i, long long j) const;
};

}; // namespace dfc
