#pragma once

// @kilasuelika
// May 19, 2022

#include "Series.hpp"
#include <eigen3/Eigen/Dense>
#include <optional>
#include <unordered_map>
#include <fstream>

namespace dfc {

template <typename IndexT, typename ColumnT> class DataFrame {
  public:
    // Constructor
    DataFrame(std::initializer_list<ColumnT> columns);
    // Blank DataFrame.
    DataFrame(const std::vector<std::string> &columns, const std::vector<DType> &types);

    // info
    const DataFrameShape &shape() const;
    std::vector<std::string> columns();

    // Subscripts.
    template <typename T> T &iloc(long long i, long long j);

    // Modify
    void insert_column(const std::string &name, DType type); // blank column
    void insert_column(ColumnT &&column);
    void insert_columns(std::vector<ColumnT> &&columns);

    // Insert a new blank row.
    void push_back();

    // conversion
    // All numeric columns to an eigen type.
    template <typename T> T to_eigen() const;
    // Specific columns.
    template <typename T> T to_eigen(std::initializer_list<std::string> l) const;

    // Data
    const std::vector<ColumnT> &values() const;
    // IO
    template <typename IndexT1, typename ColumnT1>
    friend std::ostream &operator<<(std::ostream &os,
                                    const struct DataFrame<IndexT1, ColumnT1> &dt);
    void to_csv(const std::string &filename, const CSVIOOptions &options = CSVIOOptions()) const;

    std::vector<ColumnT> _values;

  private:
    DataFrameShape _shape{0, 0};
    std::unordered_map<std::string, size_t> _column_map;

    size_t _cnt = 0; // Use this to automatically assign column name.

    std::string _find_valid_column_name(const std::string &name);
    // void _init_by_columns(std::vector<ColumnT> &&columns);
};

template <typename IndexT, typename ColumnT>
std::string DataFrame<IndexT, ColumnT>::_find_valid_column_name(const std::string &name) {
    std::string new_name = name;
    if (name.empty() or _column_map.contains(name)) {
        // Find a new column name.
        _cnt = _shape[1];
        auto max_cnt = _cnt + 1000;
        for (; _cnt < max_cnt; ++_cnt) {
            new_name = std::to_string(_cnt);
            if (not _column_map.contains(new_name)) {
                break;
            }
        }
        if (_cnt >= max_cnt) {
            std::cerr << std::format(
                             "Can find a proper new name for current name {}, please set manually.",
                             name)
                      << std::endl;
            return "";
        }
    }
    return new_name;
}

template <typename IndexT, typename ColumnT>
void DataFrame<IndexT, ColumnT>::insert_column(const std::string &name, DType type) {
    std::string new_name = _find_valid_column_name(name);
    _values.emplace_back(new_name, type);
    _column_map[new_name] = _shape[1];
    ++_shape[1];
}

template <typename IndexT, typename ColumnT>
void DataFrame<IndexT, ColumnT>::insert_column(ColumnT &&column) {
    if (_shape[1] == 0) {
        _values.emplace_back(column);
        _column_map[column.name()] = _shape[1];
        _shape[1]++;
        _shape[0] = column.size();
        return;
    }
    if (column.size() != _shape[0]) {
        std::cerr << "[insert_column]: Column " << column.name() << " has " << column.size()
                  << " rows which is not equal to " << _shape[0] << ". Failed to insert."
                  << std::endl;
        return;
    }
    if (column.name().empty() or _column_map.contains(column.name())) {
        // Find a new column name.
        _cnt = _shape[1];
        auto max_cnt = _cnt + 1000;
        for (; _cnt < max_cnt; ++_cnt) {
            std::string new_name = std::to_string(_cnt);
            if (not _column_map.contains(new_name)) {
                column.rename(new_name);
                break;
            }
        }
        if (_cnt >= max_cnt) {
            std::cerr << "Can find a proper column name, please set manually." << std::endl;
            return;
        }
    }
    _values.emplace_back(column);
    _column_map[column.name()] = _shape[1];
    _shape[1]++;
}

// template <typename IndexT, typename ColumnT>
// void dfc::DataFrame<IndexT, ColumnT>::_init_by_columns(std::vector<ColumnT> &&columns) {
//     _shape[0] = columns.size();
//     size_t sz = columns[0].size();
//     for (size_t i = 1; i < columns.size(); ++i) {
//         auto nsz = columns[i].size();
//         if (sz != columns[i].size()) {
//             std::cerr << "Column " << i << " has " << nsz << " rows which is not equal to " << sz
//                       << "." << std::endl;
//         }
//     }
//     _shape[1] = sz;
//     _values = columns;
// }

template <typename IndexT, typename ColumnT>
void DataFrame<IndexT, ColumnT>::insert_columns(std::vector<ColumnT> &&columns) {
    for (auto &&c : columns) {
        insert_column(std::move(c));
    }
}

template <typename IndexT, typename ColumnT> void DataFrame<IndexT, ColumnT>::push_back() {
    for (auto &c : _values) {
        c.push_back();
    }
    ++_shape[0];
}

template <typename IndexT, typename ColumnT>
DataFrame<IndexT, ColumnT>::DataFrame(std::initializer_list<ColumnT> columns) {
    insert_columns(columns);
}

template <typename IndexT, typename ColumnT>
DataFrame<IndexT, ColumnT>::DataFrame(const std::vector<std::string> &columns,
                                      const std::vector<DType> &types) {
    auto N = columns.size();
    if (N != types.size()) {
        std::cerr << std::format("Size of columns and types are not equal: {} and {}.", N,
                                 types.size())
                  << std::endl;
    } else {
        for (size_t i = 0; i < N; ++i) {
            insert_column(columns[i], types[i]);
        }
    }
}

template <typename IndexT, typename ColumnT>
const dfc::DataFrameShape &dfc::DataFrame<IndexT, ColumnT>::shape() const {
    return _shape;
}
template <typename IndexT, typename ColumnT>
std::vector<std::string> dfc::DataFrame<IndexT, ColumnT>::columns() {
    std::vector<std::string> res(_shape[1]);
    for (int i = 0; i < _shape[1]; ++i) {
        res[i] = _values[i].name();
    }
}

template <typename IndexT, typename ColumnT>
template <typename T>
T &DataFrame<IndexT, ColumnT>::iloc(long long i, long long j) {
    return _values[j].iloc<T>(i);
}

template <typename IndexT, typename ColumnT>
std::ostream &operator<<(std::ostream &os, const DataFrame<IndexT, ColumnT> &df) {
    // print header
    for (size_t i = 0; i < df._shape[1]; ++i) {
        os << std::setw(10) << df._values[i].name();
    }
    os << std::endl;
    // print type
    for (size_t i = 0; i < df._shape[1]; ++i) {
        os << std::setw(10) << df._values[i].dtype_name();
    }
    os << std::endl;
    for (size_t i = 0; i < df._shape[0]; ++i) {
        for (size_t j = 0; j < df._shape[1]; ++j) {
            switch (df._values[j].dtype()) {
            case DType::STRING:
                os << std::setw(10) << df._values[j].template iloc<std::string>(i);
                break;
            case DType::BOOL:
                os << std::setw(10) << df._values[j].template iloc<bool>(i);
                break;
            case DType::INT:
                os << std::setw(10) << df._values[j].template iloc<int>(i);
                break;
            case DType::LONGLONG:
                os << std::setw(10) << df._values[j].template iloc<long long>(i);
                break;
            case DType::FLOAT:
                os << std::setw(10) << df._values[j].template iloc<float>(i);
                break;
            case DType::DOUBLE:
                os << std::setw(10) << df._values[j].template iloc<double>(i);
                break;
            default:
                break;
            }
        }
        os << std::endl;
    }
    // Shape
    os << std::format("Shape: ({}, {})", df._shape[0], df._shape[1]) << std::flush;
    return os;
};

template <typename IndexT, typename ColumnT>
void DataFrame<IndexT, ColumnT>::to_csv(const std::string &filename,
                                        const CSVIOOptions &options) const {
    std::ofstream file(filename);
    // Header
    if (options._header >= 0) {
        for (size_t i = 0; i < _shape[1] - 1; ++i) {
            file << _values[i].name() << options._sep;
        }
        file << _values[_shape[1] - 1].name() << std::endl;
    }
    // Data
    for (size_t i = 0; i < _shape[0]; ++i) {
        for (size_t j = 0; j < _shape[1]; ++j) {
            switch (_values[j].dtype()) {
            case DType::STRING:
                file << _values[j].template iloc<std::string>(i);
                break;
            case DType::BOOL:
                file << _values[j].template iloc<bool>(i);
                break;
            case DType::INT:
                file << _values[j].template iloc<int>(i);
                break;
            case DType::LONGLONG:
                file << _values[j].template iloc<long long>(i);
                break;
            case DType::FLOAT:
                file << _values[j].template iloc<float>(i);
                break;
            case DType::DOUBLE:
                file << _values[j].template iloc<double>(i);
                break;
            default:
                break;
            }
            if (j < _shape[1] - 1) {
                file << options._sep;
            } else {
                file << std::endl;
            }
        }
        // os << std::endl;
    }
}

template <typename IndexT, typename ColumnT>
const std::vector<ColumnT> &dfc::DataFrame<IndexT, ColumnT>::values() const {
    return _values;
}

}; // namespace dfc