#pragma once

#include "../DataFrame.hpp"
#include "../DataFrameView.hpp"

namespace dfc {

dfc::DataFrameView::DataFrameView(DataFrame &dataframe)
    : _index(dataframe._index), _shape(dataframe.shape()), _pos(dataframe.shape()[1]),
      _column_map(dataframe._column_map) {

    std::iota(_pos.begin(), _pos.end(), 0);
    for (size_t i = 0; i < dataframe.shape()[0]; ++i) {
        _values.emplace_back(dataframe._values[i], dataframe._index);
    }
}

DataFrameView::DataFrameView(DataFrame &dataframe, const std::vector<long long> &rows,
                             const std::vector<std::string> &cols)
    : _index(dataframe._index, rows) {
    // Conpute row index.
    _shape[0] = rows.size();

    // Compute col_map;

    for (size_t i = 0; const auto &col : cols) {
        if (dataframe._column_map.contains(col)) {
            _column_map[col] = i;
            _pos.emplace_back(dataframe._column_map[col]);
            ++i;
        } else {
            std::cerr << std::format("There is no column ({}) in the original dataframe.")
                      << std::endl;
        }
    }
    _shape[1] = _column_map.size();
    //_shape({rows.size(), col.size()})
}

std::string dfc::DataFrameView::iloc_str(long long i, long long j) const {
    auto id = _cal_index(i, j);
    return iloc_str_(id[0], id[1]);
}

std::string dfc::DataFrameView::iloc_str_(size_t i, size_t j) const {
    std::ostringstream ss;
    ss << _values[j]->iloc_str(i);
    return ss.str();
}

DataFrameShape dfc::DataFrameView::_cal_index(long long i, long long j) const {
    return {i >= 0 ? i : _shape[0] + 1, j >= 0 ? j : _shape[1] + j};
}

std::ostream &dfc::operator<<(std::ostream &os, const DataFrameView &dv) {
    // print header
    // Index
    // os << std::setw(10) << df._index.name() << std::endl;
    // Column index
    os << std::setw(4) << " ";
    for (size_t i = 0; i < dv._shape[1]; ++i) {
        os << std::setw(10) << std::format("[{}]", i);
    }
    os << std::endl;

    os << std::setw(4) << " ";
    for (size_t i = 0; i < dv._shape[1]; ++i) {
        os << std::setw(10) << dv._values[i]->name();
    }
    os << std::endl;

    // print type
    // os << std::setw(10) << df._index.dtype_name();
    os << std::setw(4) << " ";
    for (size_t i = 0; i < dv._shape[1]; ++i) {
        os << std::setw(10) << dv._values[i]->dtype_name();
    }
    os << std::endl;
    for (size_t i = 0; i < dv._shape[0]; ++i) {
        os << std::setw(4) << dv._index.iloc_str(i);
        for (size_t j = 0; j < dv._shape[1]; ++j) {
            os << std::setw(10) << dv.iloc_str(i, j);
        }
        os << std::endl;
    }
    // Shape
    os << std::format("Base index: {} ({})", dv._index.name(), dv._index.dtype_name()) << std::endl;
    os << std::format("View index : {}", dv._index.name(), dv._index.type_name()) << std::endl;
    os << std::format("Shape: ({}, {})", dv._shape[0], dv._shape[1]) << std::flush;
    return os;
}

dfc::DataFrameView &dfc::DataFrameView::operator=(const DataFrameView &A) {
    if (_shape != A._shape) {
        std::cerr << "Two data frame has different shapes, failed to assignment." << std::endl;
    } else {
        for (size_t i = 0; i < _shape[1]; ++i) {
            *_values[i] = *(A._values[i]);
        }
    }
}

}; // namespace dfc