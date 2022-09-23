#pragma once

// @kilasuelika
// May 19, 2022

#include "Index.hpp"

namespace dfc {

class DataFrame;

// DataFrameView stores multiple pointers to many SeriesView.
class DataFrameView {
  public:
    DataFrameView(){};
    explicit DataFrameView(const DataFrame &dataframe);
    DataFrameView(const DataFrame &dataframe, const std::vector<std::string> &cols); 
    // rows can contains negative locations(not index).
    DataFrameView(const DataFrame &dataframe, const std::vector<long long> &rows,
                  const std::vector<std::string> &cols);
    // Both rows and cols are integer locations.
    DataFrameView(const DataFrame &dataframe, const std::vector<long long> &rows,
                  const std::vector<long long> &cols);
    template <iloc_type T1, iloc_type T2>
    DataFrameView(const DataFrameView &view, const std::vector<T1> &rows,
                  const std::vector<T2> &cols);

    //info
    DataFrameShape shape() const;
    // Copy
    DataFrame copy() const;

    //Modify
    template<typename T> void astype();

    

    friend DataFrame;

    // Subscripts
    template <typename T> T &iloc(long long i, long long j);
    template <typename T> T &iloc_(size_t i, size_t j);

    std::string iloc_str(long long i, long long j) const;
    std::string iloc_str_(size_t i, size_t j) const;

    template <iloc_type T1, iloc_type T2>
    DataFrameView iloc(const std::vector<T1> &rows, const std::vector<T2> &cols) const;
    template <iloc_type T1, iloc_type T2>
    DataFrameView iloc(std::initializer_list<T1> &rows, const std::vector<T2> &cols) const;
    template <iloc_type T1, iloc_type T2>
    DataFrameView iloc(std::initializer_list<T1> rows, std::initializer_list<T2> cols) const;

    // If negative, return all rows and cols.
    DataFrameView head(long long rows = -1, long long cols = -1) const;

    // IO
    friend std::ostream &operator<<(std::ostream &os, const DataFrameView &dv);

    // Assignment
    DataFrameView &operator=(const DataFrameView &A);
    template <typename T> DataFrameView &operator=(const T &v);

    DataFrame operator+(const DataFrameView &obj);
    DataFrame operator-(const DataFrameView &obj);
    DataFrame operator*(const DataFrameView &obj);
    DataFrame operator/(const DataFrameView &obj);

    DataFrameView& operator+=(const DataFrameView &obj);
    DataFrameView &operator-=(const DataFrameView &obj);
    DataFrameView &operator*=(const DataFrameView &obj);
    DataFrameView &operator/=(const DataFrameView &obj);

    DataFrame operator+(const DataFrame &obj);
    DataFrame operator-(const DataFrame &obj);
    DataFrame operator*(const DataFrame &obj);
    DataFrame operator/(const DataFrame &obj);

    ~DataFrameView();

  private:
    void _init_by_cols(const DataFrame &dataframe, const std::vector<std::string> &cols);
    // DataFrame *_values;
    std::shared_ptr<ViewIndex> _index;
    std::vector<SeriesView *> _values;

    DataFrameShape _shape{0,0};
    std::multimap<std::string, size_t> _column_map;

    DataFrameShape _cal_index(long long i, long long j) const;
};

}; // namespace dfc
