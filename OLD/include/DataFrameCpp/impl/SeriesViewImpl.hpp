#pragma once

// @kilasuelika
// May 19, 2022

#include "../Series.hpp"
#include "../SeriesView.hpp"

namespace dfc {
SeriesView::SeriesView(Series *series, Index &index) : _index(index), _values(series) {}

std::string SeriesView::name() const { return _values->name(); }
size_t SeriesView::size() const { return _index.size(); }
DType SeriesView::dtype() const { return _values->dtype(); }
std::string SeriesView::dtype_name() const { return _values->dtype_name(); }
bool SeriesView::empty() const { return _values->empty(); }
dfc::ViewIndexType dfc::SeriesView::index_type() const { return _index.type(); }

std::string SeriesView::iloc_str_(long long i) const {
    return _values->iloc_str_(_index.iloc_id_(i));
}
std::string SeriesView::iloc_str(long long i) const { return _values->iloc_str(_index.iloc_id(i)); }

std::ostream &operator<<(std::ostream &os, const SeriesView &df) {
    os << std::setw(4) << "" << std::setw(10) << df.name() << std::endl
       << std::setw(4) << "" << std::setw(10) << df.dtype_name() << std::endl;
    for (auto i = 0; i < df.size(); ++i) {
        os << std::setw(4) << df._index.iloc_str(i) << std::setw(10) << df.iloc_str(i) << std::endl;
    }
    os << std::format("Base index: {} ({})", df._index.name(), df._index.dtype_name()) << std::endl;
    os << std::format("View index trival: {}", df._index.is_trival()) << std::endl;
    os << "Length: " << df.size() << std::endl;
    return os;
}

template <typename T> void dfc::SeriesView::astype() { _values->astype<T>(); }

template <typename T1, typename T2>
dfc::SeriesView &dfc::SeriesView::assign(const SeriesView &obj) {
    if (DTypeMap[std::type_index(typeid(T1))] != dtype()) {
        astype<T1>();
    }
    auto &lhs = _values.vector<T1>();

    if (_values != obj._values) {
        // Two series are different.
        auto &rhs = obj.vector<T2>();
        for (size_t i = 0; i < _index.size(); ++i) {
            lhs[_index.iloc_id_(i)] = rhs[_index.iloc_id_(i)];
        }
    } else {
        // Two series are the same. Then must make a copy.
        std::vector<T2> rhs = lhs;
        for (size_t i = 0; i < _index.size(); ++i) {
            lhs[_index.iloc_id_(i)] = rhs[_index.iloc_id_(i)];
        }
    }
    return *this;
}

#include "AssignmentMacro.hpp"
SeriesView &SeriesView::operator=(const SeriesView &A) {
    auto N1 = size(), N2 = A.size();
    if (N1 != N2) {
        std::cerr << std::format("SeriesView {} and {} has different length. Can't assignment.", N1,
                                 N2)
                  << std::endl;
    } else {
        _ASSGNMENT_SWITCH_CASE_(assign, A);
    }
    return *this;
}
#include "UndefMacro.hpp"

// template <typename T> dfc::SeriesViewIterator<T>::SeriesViewIterator(SeriesView *data) :
// data(data) {
//     if (data!=nullptr) {
//         i=data->
//     }
// }
// template <typename T> T dfc::SeriesViewIterator<T>::operator*() { return *current; }
// template <typename T> dfc::SeriesViewIterator<T> &dfc::SeriesViewIterator<T>::operator++() {
//     switch (data.index_type()) {
//     case ViewIndexType::RANGE:
//         break;
//     case ViewIndexType::SLICE:
//         break;
//     case ViewIndexType::NORMAL:
//
//         break;
//     }
// }

}; // namespace dfc
