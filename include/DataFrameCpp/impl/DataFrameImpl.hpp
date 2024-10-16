#pragma once

// @kilasuelika
// May 21, 2022

#include "../DataFrame.hpp" n
#include "../DataFrameView.hpp"
// #include <fmt/format.h>
#include <iomanip>
#include "../math.hpp"
#include <unordered_set>

namespace dfc {
    inline std::string DataFrame::_find_valid_column_name(const std::string& name) {
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

    inline void DataFrame::append_col(const std::string& name, DType type) {
        // std::string new_name = _find_valid_column_name(name);

        _values.push_back(new Series(name, type));
        _values.back()->resize(_shape[0]);
        _column_map.insert(std::pair{ name, _shape[1] });
        ++_shape[1];
    }

    inline void dfc::DataFrame::insert_col(int k, const std::string& name, Series column) {
        column.rename(name);
        insert_col(k, std::move(column));
    }

    inline void dfc::DataFrame::insert_col(int k, Series&& column) {
        if (k > _shape[1]) {
            std::cerr << "k is larger than total columns, failed to insert." << std::endl;
            return;
        }
        // If insert the first column, then set row size.
        if (_shape[1] == 0) {
            _column_map.insert(std::pair{ column.name(), 0 });
            _values.push_back(new Series(std::move(column)));
            //_column_map[column.name()] = _shape[1];
            _shape[1]++;
            _shape[0] = column.size();
            return;
        }
        if (column.size() != _shape[0]) {
            std::cerr << "[append]: Column " << column.name() << " has " << column.size()
                << " rows which is not equal to " << _shape[0] << ". Failed to insert."
                << std::endl;
            return;
        }
        // std::string new_name = _find_valid_column_name(column.name());
        // column.rename(new_name);
        for (auto& [_, v] : _column_map) {
            if (v >= k)
                ++v;
        }
        _column_map.insert(std::pair{ column.name(), k });
        //_values.emplace_back(new Series(std::move(column)));
        _values.insert(_values.begin() + k, new Series(std::move(column)));
        _shape[1]++;
    }

    inline void DataFrame::append_col(Series&& column) {
        // If insert the first column, then set row size.
        if (_shape[1] == 0) {
            _column_map.insert(std::pair{ column.name(), 0 });
            _shape[0] = column.size();
            _values.push_back(new Series(std::move(column)));
            //_column_map[column.name()] = _shape[1];
            _shape[1]++;
            return;
        }
        if (column.size() != _shape[0]) {
            std::cerr << "[append]: Column " << column.name() << " has " << column.size()
                << " rows which is not equal to " << _shape[0] << ". Failed to insert."
                << std::endl;
            return;
        }
        // std::string new_name = _find_valid_column_name(column.name());
        // column.rename(new_name);
        _column_map.insert(std::pair{ column.name(), _shape[1] });
        _values.emplace_back(new Series(std::move(column)));
        _shape[1]++;
    }

    inline void dfc::DataFrame::append_col(const std::string& name, Series column) {
        column.rename(name);
        append_col(std::move(column));
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

    inline void DataFrame::append_cols(std::vector<Series>&& columns) {
        for (auto&& c : columns) {
            append_col(std::move(c));
        }
        _index->resize(_shape[0]);
    }

    template <bool inplace = false> decltype(auto) DataFrame::drop(const std::string& name) {
        auto it = _column_map.equal_range(name);
        int k = 0;
        for (auto sit = it.first; sit != it.second; ++sit) {
        }
    }

    template <bool inplace> inline decltype(auto) DataFrame::drop(int col_id) {
        if constexpr (inplace) {
            int target_id = std::numeric_limits<int>::max();
            if (-_shape[1] <= col_id && col_id < 0) {
                target_id = _shape[1] + col_id;
            }
            else if (col_id < _shape[1]) {
                target_id = col_id;
            }
            if (target_id != std::numeric_limits<int>::max) {
                _values.erase(_values.begin() + target_id);
                erase_map_by_value(_column_map, col_id, 1);
                --_shape[0];
            }
        }
        else {
            DataFrame _new = *this;
            _new.drop<true>(col_id);
            return _new;
        }
    }

    template <bool inplace> inline decltype(auto) DataFrame::drop(const std::vector<int>& col_ids) {
        if constexpr (inplace) {
            auto _col_ids = convert_index(col_ids, _shape[1]);
            auto dropped_ = erase_vec_by_ids(_values, _col_ids);
            erase_map_by_value<std::multimap<std::string, size_t>, std::unordered_set>(
                _column_map, std::unordered_set<size_t>(dropped_.begin(), dropped_.end()));
            _shape[1] -= dropped_.size();
        }
        else {
            DataFrame res = *this;
            res.drop<true>(col_ids);
            return res;
        }
    }

    inline void DataFrame::append_row() {
        for (auto& c : _values) {
            c->push_back();
        }
        ++_shape[0];
        _index->push_back();
    }

    template <bool inplace> inline decltype(auto) dfc::DataFrame::set_index(const std::string& name) {
        if constexpr (inplace) {
            if (contains_unique(name)) {
                _index = create_index(_values[get_unique_in_multimap(_column_map, name)]);
            }
            return;
        }
        else {
        }
    }

    inline DataFrame::DataFrame(std::initializer_list<Series> columns) {
        append_cols(std::move(columns));
        _index = std::make_shared<Int64RangeIndex>(_values[0]->size());
    }

    inline DataFrame::DataFrame(const std::vector<std::string>& columns,
        const std::vector<DType>& types) {
        auto N = columns.size();
        if (N != types.size()) {
            std::cerr << std::format("Size of columns and types are not equal: {} and {}.", N,
                types.size())
                << std::endl;
        }
        else {
            for (size_t i = 0; i < N; ++i) {
                append_col(columns[i], types[i]);
            }
        }
    }

    inline dfc::DataFrame::DataFrame(const DataFrame& df)
        : _shape(df._shape), _column_map(df._column_map), _index(df._index) {
        for (auto& col : df._values) {
            _values.push_back(new Series(*col));
        }
    }

    inline dfc::DataFrame::DataFrame(const DataFrameView& view)
        : _shape(view._shape), _index(view._index->clone()) {

        // Copy data
        for (int i = 0; i < view._values.size(); ++i) {
            auto& c = view._values[i];
            auto new_c = new Series(*(view._values[i]));
            // auto new_name = _find_valid_column_name(new_c->name());
            // new_c->rename(new_name);
            _column_map.insert(std::pair{ new_c->name(), i });
            //_column_map[new_name] = i;
            _values.push_back(new_c);
        }
    }

    inline dfc::DataFrame::DataFrame(std::shared_ptr<Index> index) : _index(index->clone()) {}

    // inline dfc::DataFrame::DataFrame(std::shared_ptr<ViewIndex> index)
    //     : _index(std::make_shared<Index>(*index)), _shape({index->size(), 0}) {
    //     std::cout << "DataFrame: " << _index->size() << std::endl;
    // }

    inline std::vector<dfc::DType> dfc::DataFrame::dtypes() const {
        std::vector<DType> res(_shape[1]);
        for (int i = 0; i < _shape[1]; ++i) {
            res[i] = _values[i]->dtype();
        }
        return res;
    }

    inline const DataFrameShape& DataFrame::shape() const { return _shape; }
    inline bool dfc::DataFrame::empty() const { return _shape == DataFrameShape{ 0, 0 }; }

    inline dfc::DataFrameView dfc::DataFrame::head(int n) const {
        std::vector<long long> pos(n), cols(_shape[1]);
        std::iota(pos.begin(), pos.end(), 0);
        std::iota(cols.begin(), cols.end(), 0);
        return DataFrameView(*this, pos, cols);
    }

    inline void dfc::DataFrame::describe() const {
        if (_shape[1] > 0) {
            fmt::print("{:>4}{:>12}{:>10}\n", "", "Col name", "Type");
            for (int i = 0; i < _shape[1]; ++i) {
                fmt::print("{:>4}{:>12}{:>10}\n", i, _values[i]->name(), _values[i]->dtype_name());
            }
        }
    }

    inline std::vector<std::string> DataFrame::columns() const {
        std::vector<std::string> res(_shape[1]);
        for (int i = 0; i < _shape[1]; ++i) {
            res[i] = _values[i]->name();
        }
        return res;
    }

    template <typename T> inline T& DataFrame::iloc(long long i, long long j) {
        return _values[j]->iloc<T>(i);
    }

    inline DataFrameView DataFrame::operator[](const std::string& col) const {
        auto it = _column_map.equal_range(col);
        if (it.first == it.second) {
            auto info = std::format("[DataFrame]: Can't find column {}.", col);
            std::cerr << info << std::endl;
            // throw(std::range_error(info));
            return DataFrameView();
        }
        else {
            return DataFrameView(*this, { col });
        }
    }

    inline DataFrameView DataFrame::operator[](const DataFrame& df) const {
        auto& col = df._values[0];
        switch (col->dtype()) {
        case DType::BOOL: {
            auto& v = col->vector<bool>();
            std::vector<long long> rows = where<long long>(v), cols(_shape[1]);
            std::iota(cols.begin(), cols.end(), 0);

            return DataFrameView(*this, rows, cols);
        }
        default:
            fmt::cerr(
                "Can't subscript a DataFrame with a dataframe with type {}. Valid types are: BOOL.\n",
                col->dtype_name());
            return DataFrameView();
        }
    }

    inline DataFrameView DataFrame::iloc(const std::vector<long long>& rows,
        const std::vector<long long>& cols) {
        return DataFrameView(*this, rows, cols);
    }

    inline dfc::DataFrameView dfc::DataFrame::iloc(const std::vector<long long>& rows,
        const std::vector<std::string>& cols) {
        return DataFrameView(*this, rows, cols);
    }

    inline dfc::DataFrameView dfc::DataFrame::iloc(const std::vector<long long>& rows,
        std::initializer_list<const char*> cols) {
        return iloc(rows, vec_ccharp_to_vec_str(cols));
    }

    // dfc::DataFrameView dfc::DataFrame::iloc(const std::vector<long long> &rows,
    //                                         const std::vector<const char *> &cols) {
    //     return DataFrameView(*this, rows, vec_ccharp_to_vec_str(cols));
    // }

    inline std::string dfc::DataFrame::iloc_str_(size_t i, size_t j) { return _values[j]->iloc_str(i); }

    inline std::ostream& operator<<(std::ostream& os, const DataFrame& df) {
        if (df.empty()) {
            std::cerr << "Empty DataFrame." << std::endl;
            return os;
        }
        fmt::print("df: shape {} {}\n", df._shape[0], df._shape[1]);
        os << DataFrameView(df);
        return os;
    };

    inline void DataFrame::to_csv(const std::string& filename, const CSVIOOptions& options) const {
        std::ofstream file(filename);
        if (empty()) {
            return;
        }
        DataFrameView(*this).to_csv(filename, options);
    }

    template <bool inplace, supported_functor_type Func> decltype(auto) dfc::DataFrame::apply(Func f) {
        if constexpr (inplace) {
            for (auto s : _values) {
                s->apply<true, Func>(f);
            }
            DataFrame& res = *this;
            return res;
        }
        else {
            DataFrame res(*this);
            res.apply<true, Func>(f);
            return res;
        }
    }

    template <bool inplace, template <typename> typename TemplateFunc,
        supported_type_pack... TargetArgumentTypes>
    decltype(auto) DataFrame::apply() {
        if constexpr (inplace) {
            for (auto s : _values) {
                s->apply<true, TemplateFunc, TargetArgumentTypes...>();
            }
            DataFrame& res = *this;
            return res;
        }
        else {
            DataFrame res(*this);
            res.apply<true, TemplateFunc, TargetArgumentTypes...>();
            return res;
        }
    };

    inline const std::vector<Series*>& DataFrame::values() const { return _values; }

    inline DataFrame dfc::DataFrame::copy() const {
        DataFrame res;
        return res;
    }

    inline bool dfc::DataFrame::all_numeric_dtype() const {
        for (const auto& c : _values) {
            if (!c->is_numeric_dtype())
                return false;
        }
        return true;
    }

    template <typename T> T inline dfc::DataFrame::to_eigen() const { return to_eigen<T>(columns()); }

    template <typename T> inline T DataFrame::to_eigen(const std::vector<std::string>& l) const {
        // Count cols.
        int total_columns = 0;
        for (auto& cn : l) {
            total_columns += _column_map.count(cn);
        }
        T res(shape()[0], total_columns);

        using EleType = typename T::Scalar;

        int i = 0;
        for (auto& col_name : l) {
            auto it = _column_map.equal_range(col_name);
            if (it.first != it.second) {
                for (auto sit = it.first; sit != it.second; ++sit) {
                    auto col_data = _values[sit->second]->asvector<EleType>();
                    std::copy(col_data.begin(), col_data.end(), res.col(i).begin());
                    ++i;
                }
            }
            else {
                std::cerr << std::format("This dataframe doesn't have a column {}.", col_name)
                    << std::endl;
            }
        }
        return res;
    }

#define _DATAFRAME_BIN_OP_IMPL(op)                                                                 \
    inline DataFrame operator op(const DataFrame &lhs, const DataFrame &rhs) {                     \
        if (lhs._shape != rhs._shape) {                                                            \
            std::string info =                                                                     \
                std::format("[DataFrame]: two data frame has different shape: ({}, {}), ({},{}).", \
                            lhs._shape[0], lhs._shape[1], rhs._shape[0], rhs._shape[1]);           \
            std::cerr << info << std::endl;                                                        \
            throw(std::runtime_error(info));                                                       \
            return DataFrame();                                                                    \
        } else {                                                                                   \
            DataFrame df(lhs._index);                                                              \
            for (int i = 0; i < lhs._shape[1]; ++i) {                                              \
                df.append_col(std::move(*(lhs._values[i])op *(rhs._values[i])));                   \
            }                                                                                      \
            return df;                                                                             \
        }                                                                                          \
    }                                                                                              \
    inline DataFrame operator op(const DataFrame &lhs, const DataFrameView &rhs) {                 \
        if (lhs._shape != rhs._shape) {                                                            \
            std::string info =                                                                     \
                std::format("[DataFrame]: DataFrame and DataFrameView: ({}, {}), ({},{}).",        \
                            lhs._shape[0], lhs._shape[1], rhs._shape[0], rhs._shape[1]);           \
            std::cerr << info << std::endl;                                                        \
            throw(std::runtime_error(info));                                                       \
            return DataFrame();                                                                    \
        } else {                                                                                   \
            DataFrame df(lhs._index->clone());                                                     \
            for (int i = 0; i < lhs._shape[1]; ++i) {                                              \
                df.append_col(std::move(*(lhs._values[i])op *(rhs._values[i])));                   \
            }                                                                                      \
            return df;                                                                             \
        }                                                                                          \
    }                                                                                              \
    inline DataFrame operator op(const DataFrameView &rhs, const DataFrame &lhs) {                 \
        return lhs op rhs;                                                                         \
    }

    _DATAFRAME_BIN_OP_IMPL(+);
    _DATAFRAME_BIN_OP_IMPL(-);
    _DATAFRAME_BIN_OP_IMPL(*);
    _DATAFRAME_BIN_OP_IMPL(/ );

#define DataFrameCpp_DataFrame_SelfBinOP_DataFrameView(op)                                         \
    inline DataFrame &DataFrame::operator op(const DataFrameView &rhs) {                           \
        for (int i = 0; i < _values.size(); ++i) {                                                 \
            *(_values[i]) += *(rhs._values[i]);                                                    \
        }                                                                                          \
        return *this;                                                                              \
    }

    DataFrameCpp_DataFrame_SelfBinOP_DataFrameView(+= );
    DataFrameCpp_DataFrame_SelfBinOP_DataFrameView(-= );
    DataFrameCpp_DataFrame_SelfBinOP_DataFrameView(*= );
    DataFrameCpp_DataFrame_SelfBinOP_DataFrameView(/= );
#undef DataFrameCpp_DataFrame_SelfBinOP_DataFrameView

#include "../macro/dataframe_dataframeview_op.h"

    DataFrameCpp_DataFrame_Or_View_Scalar_Bin_Op(DataFrame, < );
    DataFrameCpp_DataFrame_Or_View_Scalar_Bin_Op(DataFrame, > );
    DataFrameCpp_DataFrame_Or_View_Scalar_Bin_Op(DataFrame, >= );
    DataFrameCpp_DataFrame_Or_View_Scalar_Bin_Op(DataFrame, <= );
    DataFrameCpp_DataFrame_Or_View_Scalar_Bin_Op(DataFrame, == );
    DataFrameCpp_DataFrame_Or_View_Scalar_Bin_Op(DataFrame, != );
    DataFrameCpp_DataFrame_Or_View_Scalar_Bin_Op(DataFrame, &&);
    DataFrameCpp_DataFrame_Or_View_Scalar_Bin_Op(DataFrame, || );

#include "../macro/undef_dataframe_dataframeview_op.h"

    inline dfc::DataFrame::~DataFrame() {
        // std::cout << "Destroy dataframe." << std::endl;
        for (auto& p : _values)
            delete p;
    }
}; // namespace dfc
