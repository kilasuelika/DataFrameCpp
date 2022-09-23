#pragma once

// @kilasuelika
// May 21, 2022

#include "../DataFrame.hpp"
#include "../DataFrameView.hpp"

namespace dfc {
std::string DataFrame::_find_valid_column_name(const std::string &name) {
    std::string new_name = name;
    if (name.empty() or _column_map.contains(name)) {
        // Find a new column name.
        int _cnt = 1;
        auto max_cnt = _cnt + 1000;
        for (; _cnt < max_cnt; ++_cnt) {
            new_name = std::format("{}__{}", name, _cnt);
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

void DataFrame::append_column(const std::string &name, DType type) {
    //std::string new_name = _find_valid_column_name(name);
    _values.push_back(new Series(name, type));
    _values.back()->resize(_shape[0]);
    _column_map.insert(std::pair{name, _shape[1]});
    ++_shape[1];
}

void dfc::DataFrame::insert_column(int k, const std::string &name, Series column) {
    column.rename(name);
    insert_column(k, std::move(column));
}
void dfc::DataFrame::insert_column(int k, Series &&column) {
    if (k > _shape[1]) {
        std::cerr << "k is larger than total columns, failed to insert_column." << std::endl;
        return;
    }
    // If insert the first column, then set row size.
    if (_shape[1] == 0) {
        _column_map.insert(std::pair{column.name(), 0});
        _values.push_back(new Series(std::move(column)));
        //_column_map[column.name()] = _shape[1];
        _shape[1]++;
        _shape[0] = column.size();
        return;
    }
    if (column.size() != _shape[0]) {
        std::cerr << "[append_column]: Column " << column.name() << " has " << column.size()
                  << " rows which is not equal to " << _shape[0] << ". Failed to insert."
                  << std::endl;
        return;
    }
    // std::string new_name = _find_valid_column_name(column.name());
    // column.rename(new_name);
    for (auto& [_,v]:_column_map) {
        if (v >= k)
            ++v;    
    }
    _column_map.insert(std::pair{column.name(), k});
    //_values.emplace_back(new Series(std::move(column)));
    _values.insert(_values.begin()+k, new Series(std::move(column)));
    _shape[1]++;
}
void DataFrame::append_column(Series &&column) { // If insert the first column, then set row size.
    if (_shape[1] == 0) {
        _column_map.insert(std::pair{column.name(), 0});
        _values.push_back(new Series(std::move(column)));
        //_column_map[column.name()] = _shape[1];
        _shape[1]++;
        _shape[0] = column.size();
        return;
    }
    if (column.size() != _shape[0]) {
        std::cerr << "[append_column]: Column " << column.name() << " has " << column.size()
                  << " rows which is not equal to " << _shape[0] << ". Failed to insert."
                  << std::endl;
        return;
    }
    // std::string new_name = _find_valid_column_name(column.name());
    // column.rename(new_name);
    _column_map.insert(std::pair{column.name(), _shape[1]});
    _values.emplace_back(new Series(std::move(column)));
    _shape[1]++;
}

void dfc::DataFrame::append_column(const std::string &name, Series column) {
    column.rename(name);
    append_column(std::move(column));
}
//
// void DataFrame::_init_by_columns(std::vector<Series> &&columns) {
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

void DataFrame::append_columns(std::vector<Series> &&columns) {
    for (auto &&c : columns) {
        append_column(std::move(c));
    }
    _index.resize(_shape[0]);
}


void DataFrame::append_row() {
    for (auto &c : _values) {
        c->push_back();
    }
    ++_shape[0];
    _index.push_back();
}

DataFrame::DataFrame(std::initializer_list<Series> columns) { append_columns(columns); }

DataFrame::DataFrame(const std::vector<std::string> &columns, const std::vector<DType> &types) {
    auto N = columns.size();
    if (N != types.size()) {
        std::cerr << std::format("Size of columns and types are not equal: {} and {}.", N,
                                 types.size())
                  << std::endl;
    } else {
        for (size_t i = 0; i < N; ++i) {
            append_column(columns[i], types[i]);
        }
    }
}

dfc::DataFrame::DataFrame(const DataFrame &df)
    : _shape(df._shape), _column_map(df._column_map), _index(df._index) {
    for (auto &col : df._values) {
        _values.push_back(new Series(*col));
    }
}

dfc::DataFrame::DataFrame(const DataFrameView &view) : _shape(view._shape), _index(*(view._index)) {

    // Copy data
    for (int i = 0; i < view._values.size(); ++i) {
        auto &c = view._values[i];
        auto new_c = new Series(*(view._values[i]));
        //auto new_name = _find_valid_column_name(new_c->name());
        //new_c->rename(new_name);
        _column_map.insert(std::pair{new_c->name(), i});
        //_column_map[new_name] = i;
        _values.push_back(new_c);
    }
}

const DataFrameShape &DataFrame::shape() const { return _shape; }

std::vector<std::string> DataFrame::columns() const {
    std::vector<std::string> res(_shape[1]);
    for (int i = 0; i < _shape[1]; ++i) {
        res[i] = _values[i]->name();
    }
}

template <typename T> T &DataFrame::iloc(long long i, long long j) { return _values[j]->iloc<T>(i); }

DataFrameView DataFrame::operator[](const std::string &col) const {
    auto it = _column_map.equal_range(col);
    if (it .first==it.second) {
        auto info = std::format("[DataFrame]: Can't find column {}.", col);
        std::cerr << info << std::endl;
        // throw(std::range_error(info));
        return DataFrameView();
    } else {
        return DataFrameView(*this, {col});
    }
}

dfc::DataFrameView dfc::DataFrame::iloc(const std::vector<long long> &rows,
                                        const std::vector<long long> &cols) {
    return DataFrameView(*this, rows, cols);
}

inline std::string dfc::DataFrame::iloc_str_(size_t i, size_t j) { return _values[j]->iloc_str(i); }

std::ostream &operator<<(std::ostream &os, const DataFrame &df) {
    // print header
    // Index
    // os << std::setw(10) << df._index.name() << std::endl;
    os << std::setw(4) << " ";
    for (size_t i = 0; i < df._shape[1]; ++i) {
        os << std::setw(10) << std::format("[{}]", i);
    }
    os << std::endl;

    os << std::setw(4) << " ";
    for (size_t i = 0; i < df._shape[1]; ++i) {
        os << std::setw(10) << df._values[i]->name();
    }
    os << std::endl;
    // print type
    // os << std::setw(10) << df._index.dtype_name();
    os << std::setw(4) << " ";
    for (size_t i = 0; i < df._shape[1]; ++i) {
        os << std::setw(10) << df._values[i]->dtype_name();
    }
    os << std::endl;
    for (size_t i = 0; i < df._shape[0]; ++i) {
        os << std::setw(4) << df._index.iloc_str(i);
        for (size_t j = 0; j < df._shape[1]; ++j) {
            os << std::setw(10) << df._values[j]->iloc_str(i);
        }
        os << std::endl;
    }
    // Shape
    os << std::format("Index: {} ({})", df._index.name(), df._index.dtype_name()) << std::endl;
    os << std::format("Shape: ({}, {})", df._shape[0], df._shape[1]) << std::flush;
    return os;
};

void DataFrame::to_csv(const std::string &filename, const CSVIOOptions &options) const {
    std::ofstream file(filename);
    // Header
    if (options._header >= 0) {
        file << _index.name() << ",";
        for (size_t i = 0; i < _shape[1] - 1; ++i) {
            file << _values[i]->name() << options._sep;
        }
        file << _values[_shape[1] - 1]->name() << std::endl;
    }
    // Data
    for (size_t i = 0; i < _shape[0]; ++i) {
        file << _index.iloc_str(i) << ",";
        for (size_t j = 0; j < _shape[1]; ++j) {
            file << _values[j]->iloc_str(i);
            if (j < _shape[1] - 1) {
                file << options._sep;
            } else {
                file << std::endl;
            }
        }
        // os << std::endl;
    }
}

const std::vector<Series *> &DataFrame::values() const { return _values; }

DataFrame dfc::DataFrame::copy() const {
    DataFrame res;
    return res;
}

template <typename T> T dfc::DataFrame::to_eigen() const { return to_eigen<T>(columns()); }

template <typename T> T DataFrame::to_eigen(const std::vector<std::string> &l) const {
    //Count cols.
    int total_columns=0;
    for (auto & cn: l) {
        total_columns += _column_map.count(cn);
    }
    T res(shape()[0], total_columns);

    using EleType = typename T::Scalar;

    int i = 0;
    for (auto &col_name : l) {
        auto it = _column_map.equal_range(col_name);
        if (it.first!=it.second) {
            for (auto sit=it.first;sit!=it.second;++sit) {
                auto col_data = _values[sit->second]->asvector<EleType>();
                std::copy(col_data.begin(), col_data.end(), res.col(i).begin());
                ++i;
            }
        }else {
            std::cerr << std::format("This dataframe doesn't have a column {}.", col_name)
                      << std::endl;
        }

        
    }
    return res;
}

_DATAFRAME_BIN_OP_IMPL(+);
_DATAFRAME_BIN_OP_IMPL(-);
_DATAFRAME_BIN_OP_IMPL(*);
_DATAFRAME_BIN_OP_IMPL(/);

dfc::DataFrame::~DataFrame() {
    for (auto &p : _values)
        delete p;
}
}; // namespace dfc