#pragma once

#include "../DataFrame.hpp"
#include "../DataFrameView.hpp"
#include <set>

namespace dfc {

dfc::DataFrameView::DataFrameView(const DataFrame &dataframe)
    : _index(std::make_shared<ViewIndex>(const_cast<Index *>(&dataframe._index))),
      _shape(dataframe.shape()), _column_map(dataframe._column_map) {

    for (size_t i = 0; i < dataframe.shape()[0]; ++i) {
        _values.push_back(new SeriesView(dataframe._values[i]));
    }
}

DataFrameView::DataFrameView(const DataFrame &dataframe, const std::vector<std::string> &cols)
    : _index(std::make_shared<ViewIndex>(const_cast<Index *>(&dataframe._index))) {
    _init_by_cols(dataframe, cols);
}

void dfc::DataFrameView::_init_by_cols(const DataFrame &dataframe,
                                       const std::vector<std::string> &cols) {
    int k = 0;
    for (const auto &col : cols) {
        auto it = dataframe._column_map.equal_range(col);
        for (auto sit = it.first; sit != it.second; ++sit) {
            _column_map.insert(std::pair{sit->first, k});
            _values.emplace_back(new SeriesView(dataframe._values[sit->second], _index));
            ++k;
        }
    }
    _shape[0] = dataframe.shape()[0];
    _shape[1] = k;
}
DataFrameView::DataFrameView(const DataFrame &dataframe, const std::vector<long long> &rows,
                             const std::vector<std::string> &cols)
    : _index(std::make_shared<ViewIndex>(const_cast<Index *>(&dataframe._index), rows)) {
    // Conpute row index.
    _shape[0] = rows.size();

    // auto _rows(rows);
    auto &_rows = _index->_pos;

    // Compute col_map;
    for (size_t i = 0; const auto &col : cols) {
        auto it = dataframe._column_map.equal_range(col);
        if (it.first != it.second) {
            for (auto sit = it.first; sit != it.second; ++sit) {
                _column_map.insert(std::pair{col, i});
                _values.push_back(new SeriesView(dataframe._values[sit->second], _rows));
                ++i;
            }
        } else {
            std::cerr << std::format(
                             "There is no column ({}) in the original dataframe. It's ignored.",
                             col)
                      << std::endl;
        }
    }
    _shape[1] = _column_map.size();
    //_shape({rows.size(), col.size()})
}
dfc::DataFrameView::DataFrameView(const DataFrame &dataframe, const std::vector<long long> &rows,
                                  const std::vector<long long> &cols)
    : _index(std::make_shared<ViewIndex>(const_cast<Index *>(&dataframe._index), rows)),
      _shape({rows.size(), cols.size()}) {
    auto &_rows = _index->_pos;

    std::set<size_t> s;
    for (size_t i = 0; const auto &col : cols) {
        //_column_map[dataframe._values[i]->name()] = i;
        _column_map.insert(std::pair{dataframe._values[i]->name(), i});

        size_t c = col >= 0
                       ? static_cast<size_t>(col)
                       : static_cast<size_t>(static_cast<long long>(dataframe.shape()[1]) + col);
        _values.push_back(new SeriesView(dataframe._values[c], _rows));
    }
}
template <iloc_type T1, iloc_type T2>
DataFrameView::DataFrameView(const DataFrameView &view, const std::vector<T1> &rows,
                             const std::vector<T2> &cols)
    : _index(view._index, rows), _shape({rows.size(), cols.size()}) {
    auto &_rows = _index->_pos;

    std::set<size_t> s;
    for (size_t i = 0; const auto &col : cols) {
        _column_map.insert(std::pair{view._values[i]->name(), i});
        size_t c;
        if constexpr (std::is_same_v<T2, size_t>) {
            c = col;
        } else {
            c = col >= 0 ? static_cast<size_t>(col)
                         : static_cast<size_t>(static_cast<long long>(_shape[1]) + col);
        }

        _values.push_back(new SeriesView(view._values[c], _rows));
    }
}

dfc::DataFrameShape dfc::DataFrameView::shape() const { return _shape; }

dfc::DataFrame dfc::DataFrameView::copy() const { return DataFrame(*this); }

template <typename T> void dfc::DataFrameView::astype() { for (auto& c: _values) {
        c->astype<T>();
    } }

template <typename T> T &dfc::DataFrameView::iloc(long long i, long long j) {
    auto id = _cal_index(i, j);
    return iloc_<T>(id[0], id[1]);
}
template <typename T> T &dfc::DataFrameView::iloc_(size_t i, size_t j) {
    return _values[j]->iloc_<T>(i);
}
std::string dfc::DataFrameView::iloc_str(long long i, long long j) const {
    auto id = _cal_index(i, j);
    return iloc_str_(id[0], id[1]);
}

std::string dfc::DataFrameView::iloc_str_(size_t i, size_t j) const {
    return _values[j]->iloc_str(i);
}

template <iloc_type T1, iloc_type T2>
dfc::DataFrameView dfc::DataFrameView::iloc(const std::vector<T1> &rows,
                                            const std::vector<T2> &cols) const {
    return DataFrameView(*this, rows, cols);
}

template <iloc_type T1, iloc_type T2>
dfc::DataFrameView dfc::DataFrameView::iloc(std::initializer_list<T1> rows,
                                            std::initializer_list<T2> cols) const {
    return iloc(std::vector<T1>(rows), std::vector<T2>(cols));
}

dfc::DataFrameView dfc::DataFrameView::head(long long rows, long long cols) const {
    DataFrameView res(*this);
    return res;
}

DataFrameShape dfc::DataFrameView::_cal_index(long long i, long long j) const {
    return {i >= 0 ? static_cast<size_t>(i)
                   : static_cast<size_t>(static_cast<long long>(_shape[0]) + i),
            j >= 0 ? static_cast<size_t>(j)
                   : static_cast<size_t>(static_cast<long long>(_shape[1]) + j)};
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
        os << std::setw(4) << dv._index->iloc_str(i);
        for (size_t j = 0; j < dv._shape[1]; ++j) {
            os << std::setw(10) << dv.iloc_str_(i, j);
        }
        os << std::endl;
    }
    // Shape
    os << std::format("Base index: {} ({})", dv._index->name(), dv._index->dtype_name())
       << std::endl;
    os << std::format("View index : {}", dv._index->name(), dv._index->type_name()) << std::endl;
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
    return *this;
}

template <typename T> dfc::DataFrameView &dfc::DataFrameView::operator=(const T &v) {
    if constexpr (std::is_arithmetic_v<T>) {
        for (auto &c : _values) {
            c->operator=<T>(v);
        }
    }
    return *this;
}
#define DataFrameCPP_DataFrameView_BIN_ASSIGN_OP(INPUT_TYPE, OP)                                   \
    DataFrameView &DataFrameView::operator OP(const INPUT_TYPE &new_df) {                          \
        if (new_df._shape != _shape) {                                                             \
            std::cerr << "Two data frame has different shape." << std::endl;                       \
        } else {                                                                                   \
            for (int i = 0; i < _shape[1]; ++i) {                                                  \
                *(_values[i])OP *(new_df._values[i]);                                              \
            }                                                                                      \
        }                                                                                          \
        return *this;                                                                              \
    }
DataFrameCPP_DataFrameView_BIN_ASSIGN_OP(DataFrameView, +=);
DataFrameCPP_DataFrameView_BIN_ASSIGN_OP(DataFrameView, -=);
DataFrameCPP_DataFrameView_BIN_ASSIGN_OP(DataFrameView, /=);
DataFrameCPP_DataFrameView_BIN_ASSIGN_OP(DataFrameView, *=);

#undef DataFrameCPP_DataFrameView_BIN_ASSIGN_OP

dfc::DataFrameView::~DataFrameView() {
    for (auto &p : _values)
        delete p;
}
}; // namespace dfc