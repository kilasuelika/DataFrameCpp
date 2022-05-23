#pragma once

// @kilasuelika
// May 19, 2022

#include "Index.hpp"
#include <span>

namespace dfc {

template <typename T> struct SeriesViewIterator;

class SeriesView {
  public:
    SeriesView() {}
    SeriesView(Series *series, Index &index);

    // info
    std::string name() const;
    size_t size() const; // size of _index
    DType dtype() const;
    std::string dtype_name() const;
    bool empty() const;
    ViewIndexType index_type() const;

    // Subscript
    std::string iloc_str_(long long i) const;
    std::string iloc_str(long long i) const;
    // std::ostream &operator<<(std::ostream &os, const SeriesView &df);
    // template <typename T> SeriesViewIterator<T> begin() const;
    // template <typename T> SeriesViewIterator<T> end() const;

    // Modification
    template <typename T> void astype();
    // Assignment
    SeriesView &operator=(const SeriesView &obj);
    template <typename T1, typename T2> SeriesView &assign(const SeriesView &obj);

    friend std::ostream &operator<<(std::ostream &os, const SeriesView &df);
    template <typename T> friend class SeriesViewIterator;

  private:
    ViewIndex _index;
    Series *_values;
};

template <typename T> struct SeriesViewIterator {
    SeriesViewIterator(SeriesView *data);

    SeriesViewIterator &operator++();

    T operator*();

    SeriesView *data;
    T *current;
    size_t i;
};
}; // namespace dfc
