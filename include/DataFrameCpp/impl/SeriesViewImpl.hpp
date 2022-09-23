#pragma once

// @kilasuelika
// May 19, 2022

#include "../Series.hpp"
#include "../SeriesView.hpp"
#include <iomanip>

namespace dfc {
inline SeriesView::SeriesView(Series *series) : _index(std::make_shared<ViewIndex>(series->size())), _values(series) {}

inline dfc::SeriesView::SeriesView(Series *series, const std::vector<size_t> &pos)
    : _values(series), _index(std::make_shared<ViewIndex>(pos)) {}
inline SeriesView::SeriesView(Series *series, const std::vector<long long> &pos)
    : _values(series), _index(std::make_shared<ViewIndex>(series->_cal_index(pos))) {}
inline dfc::SeriesView::SeriesView(const SeriesView *view, const std::vector<size_t> &pos)
    : _values(view->_values), _index(std::make_shared<ViewIndex>(view->_index->_index, pos)) {}

inline SeriesView::SeriesView(Series *series, Index *index) : _values(series), _index(std::make_shared<ViewIndex>(index)) {
    //  std::cout << "index size: " << _index.size() << std::endl;
}
dfc::SeriesView::SeriesView(Series *series, std::shared_ptr<ViewIndex> &index)
    : _values(series), _index(index) {}

inline std::string SeriesView::name() const { return _values->name(); }
inline size_t SeriesView::size() const { return _index->size(); }
inline DType SeriesView::dtype() const { return _values->dtype(); }
inline std::string SeriesView::dtype_name() const { return _values->dtype_name(); }
inline bool SeriesView::empty() const { return _values->empty(); }
inline dfc::ViewIndexType dfc::SeriesView::index_type() const { return _index->type(); }

template <typename T> T &dfc::SeriesView::iloc_(size_t i) {
    return const_cast<T &>(const_cast<const SeriesView *>(this)->iloc_<T>(i));
}
template <typename T> inline const T &dfc::SeriesView::iloc_(size_t i) const {
    return _values->iloc_<T>(_index->iloc_id_(i));
}

inline std::string SeriesView::iloc_str_(long long i) const {
    return _values->iloc_str_(_index->iloc_id_(i));
}

inline std::string SeriesView::iloc_str(long long i) const {
    return _values->iloc_str(_index->iloc_id(i));
}

inline std::ostream &operator<<(std::ostream &os, const SeriesView &dv) {
    os << std::setw(4) << "" << std::setw(10) << dv.name() << std::endl
       << std::setw(4) << "" << std::setw(10) << dv.dtype_name() << std::endl;
    for (auto i = 0; i < dv.size(); ++i) {
        os << std::setw(4) << dv._index->iloc_str(i) << std::setw(10) << dv.iloc_str(i)
           << std::endl;
    }
    os << std::format("Base index: {} ({})", dv._index->name(), dv._index->dtype_name())
       << std::endl;
    os << std::format("View index trival: {}", dv._index->is_trival()) << std::endl;
    os << "Length: " << dv.size() << std::flush;
    return os;
}

template <typename T> void dfc::SeriesView::astype() { _values->astype<T>(); }

dfc::Series dfc::SeriesView::copy() const { return Series(*this); }

template <typename T> const std::vector<T> &dfc::SeriesView::vector() const {
    return _values->vector<T>();
}
template <typename T> std::vector<T> &dfc::SeriesView::_vector() { return _values->_vector<T>(); }

template <typename T1, typename T2>
dfc::SeriesView &dfc::SeriesView::assign(const SeriesView &obj) {
    if (DTypeMap[std::type_index(typeid(T1))] != dtype()) {
        astype<T1>();
    }
    auto &lhs = _vector<T1>();
    if (_values != obj._values) {
        // Two series are different.
        const auto &rhs = obj.vector<T2>();
        for (size_t i = 0; i < _index->size(); ++i) {
            lhs[_index->iloc_id_(i)] = rhs[obj._index->iloc_id_(i)];
        }
    } else {
        // Two series are the same. Then must make a copy.
        std::vector<T1> rhs = lhs;
        for (size_t i = 0; i < _index->size(); ++i) {
            lhs[_index->iloc_id_(i)] = rhs[obj._index->iloc_id_(i)];
        }
    }
    return *this;
}

#include "AssignmentMacro.hpp"

DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(SeriesView, SeriesView, +=, add_assignment);
DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(SeriesView, SeriesView, -=, sub_assignment);
DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(SeriesView, SeriesView, *=, mul_assignment);
DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(SeriesView, SeriesView, /=, div_assignment);

DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(SeriesView, Series, +=, add_assignment);
DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(SeriesView, Series, -=, sub_assignment);
DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(SeriesView, Series, *=, mul_assignment);
DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(SeriesView, Series, /=, div_assignment);

SeriesView &SeriesView::operator=(const SeriesView &obj) {
    auto N1 = size(), N2 = obj.size();
    if (N1 != N2) {
        std::cerr << std::format("SeriesView {} and {} has different length. Can't assignment.", N1,
                                 N2)
                  << std::endl;
    } else {
        DataFrameCpp_ASSGNMENT_SWITCH_CASE_(assign, );
    }
    return *this;
}

#define DataFrameCpp_SeriesView_Assign_Single_Value( type)                                \
   {                                                                                              \
        type x(0);                                                                       \
        using Type = decltype(x + T());                                              \
        if constexpr (!std::is_same_v<type, Type>) {                                           \
            astype<Type>();                                                                    \
        }                                                                                          \
        for (int i = 0; i < _index->size(); ++i) {                                                 \
            _values->iloc<Type>(i) = v;                                                        \
        } break;\
} 

#define DataFrameCpp_SeriesView_Assign_Single_Value_Loop(type,op)                                                       \
    for (int i = 0; i < _index->size(); ++i) {                                                     \
        _values->iloc<type>(i)=op;                                                                      \
    }
template <typename T> SeriesView &dfc::SeriesView::operator=(const T &v) {
    if constexpr (std::is_arithmetic_v<T>) {
        switch (dtype()) {
        case DType::NONE:
            break;
        case DType::STRING:
            DataFrameCpp_SeriesView_Assign_Single_Value_Loop(std::string, std::to_string(v));
            break;
        case DType::BOOL:
            DataFrameCpp_SeriesView_Assign_Single_Value(bool);
        case DType::INT:
            DataFrameCpp_SeriesView_Assign_Single_Value( int);
        case DType::LONGLONG:
            DataFrameCpp_SeriesView_Assign_Single_Value( long long);
        case DType::FLOAT:
            DataFrameCpp_SeriesView_Assign_Single_Value(float);
        case DType::DOUBLE:
            DataFrameCpp_SeriesView_Assign_Single_Value( double);
        default:
            break;
        }
    }
    return *this;
}
#undef DataFrameCpp_SeriesView_Assign_Single_Value_Loop
#undef DataFrameCpp_SeriesView_Assign_Single_Value

DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(SeriesView, SeriesView, add, +);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(SeriesView, SeriesView, sub, -);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(SeriesView, SeriesView, mul, *);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(SeriesView, SeriesView, div, /);

DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(SeriesView, Series, add, +);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(SeriesView, Series, sub, -);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(SeriesView, Series, mul, *);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(SeriesView, Series, div, /);

DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(SeriesView, SeriesView, add, +);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(SeriesView, SeriesView, sub, -);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(SeriesView, SeriesView, mul, *);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(SeriesView, SeriesView, div, /);

DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(SeriesView, Series, add, +);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(SeriesView, Series, sub, -);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(SeriesView, Series, mul, *);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(SeriesView, Series, div, /);

DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(SeriesView, SeriesView, +=, add);
DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(SeriesView, SeriesView, -=, sub);
DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(SeriesView, SeriesView, *=, mul);
DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(SeriesView, SeriesView, /=, div);

DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(SeriesView, Series, +=, add);
DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(SeriesView, Series, -=, sub);
DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(SeriesView, Series, *=, mul);
DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(SeriesView, Series, /=, div);

#include "UndefMacro.hpp"

}; // namespace dfc
