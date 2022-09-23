#pragma once

// @kilasuelika
// May 19, 2022

#include "SeriesView.hpp"
#include <eigen3/Eigen/Dense>
//#include <optional>
//#include <unordered_map>
#include <fstream>
#include <map>
#include "Index.hpp"

// Keep index of left operand.
#define _DATAFRAME_BIN_OP_IMPL(op)                                                                 \
    DataFrame DataFrame::operator op(DataFrame const &obj) {                                       \
        if (_shape != obj._shape) {                                                                \
            std::string info =                                                                     \
                std::format("[DataFrame]: two data frame has different shape: ({}, {}), ({},{}).", \
                            _shape[0], _shape[1], obj._shape[0], obj._shape[1]);                   \
            std::cerr << info << std::endl;                                                        \
            throw(std::runtime_error(info));                                                       \
            return DataFrame();                                                                    \
        } else {                                                                                   \
            DataFrame df(*this);                                                                   \
            for (size_t i = 0; i < _values.size(); ++i) {                                          \
                *(df._values[i])op## = *(obj._values[i]);                                          \
            }                                                                                      \
            return df;                                                                             \
        }                                                                                          \
    }

namespace dfc {

class DataFrameView;

class DataFrame {
  public:
    friend DataFrameView;

    // Constructor
    DataFrame(){};
    DataFrame(std::initializer_list<Series> columns);
    // Blank DataFrame.
    DataFrame(const std::vector<std::string> &columns, const std::vector<DType> &types);

    DataFrame(const DataFrame &df);
    DataFrame(const DataFrameView &view);

    // info
    const DataFrameShape &shape() const;
    std::vector<std::string> columns() const;

    // Subscripts.
    // SeriesView operator[](const std::string &col);
    DataFrameView operator[](const std::string &col) const;

    template <typename T> T &iloc(long long i, long long j);
    // SeriesView iloc(const std::vector<long long> &rows, const std::string &col);
    DataFrameView iloc(const std::vector<long long> &rows, const std::vector<long long> &cols);
    std::string iloc_str_(size_t i, size_t j); // Always positive integer.

    // Modify
    void insert_column(int k, const std::string&name, DType type);
    void insert_column(int k, Series &&column);
    void insert_column(int k, const std::string &name, Series column);

    void append_column(const std::string &name, DType type); // blank column
    void append_column(Series &&column);
    void append_column(const std::string &name, Series column);
    void append_columns(std::vector<Series> &&columns);

    // Insert a new blank row.
    void append_row();

    // Arithmetic
    DataFrame operator+(DataFrame const &obj);
    DataFrame operator-(DataFrame const &obj);
    DataFrame operator*(DataFrame const &obj);
    DataFrame operator/(DataFrame const &obj);

    // conversion
    DataFrame copy() const;
    // All numeric columns to an eigen type.
    template <typename T> T to_eigen() const;
    // Specific columns.
    template <typename T> T to_eigen(const std::vector<std::string> &l) const;

    // Data
    const std::vector<Series *> &values() const;
    // const std::unordered_map<std::string, size_t> &get_column_map() const;
    //  IO
    friend std::ostream &operator<<(std::ostream &os, const struct DataFrame &dt);
    void to_csv(const std::string &filename, const CSVIOOptions &options = CSVIOOptions()) const;

    std::vector<Series *> _values;
    Index _index;

    ~DataFrame();

  private:
    DataFrameShape _shape{0, 0};
    std::multimap<std::string, size_t> _column_map;

    std::string _find_valid_column_name(const std::string &name);
    // void _init_by_columns(std::vector<Series> &&columns);
};

}; // namespace dfc