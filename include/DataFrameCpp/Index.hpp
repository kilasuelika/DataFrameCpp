#pragma once

#include "Series.hpp"

namespace dfc {
class ViewIndex;
class SeriesView;

class DataFrame;
class DataFrameView;

// Int64Positional index.
class Index {
  public:
    Index() {}
    explicit Index(size_t sz) : _size(sz) {}
    explicit Index(const std::vector<size_t> &pos_) : _pos(pos_) {}
    Index(const std::shared_ptr<Index> &index, const std::vector<long long> &pos_)
        : _values(index->_values), _size(pos_.size()), _pos(index->iloc_id(pos_)) {
    } // Can contains negative integer.
    Index(const std::shared_ptr<Index> &index, const std::vector<size_t> &pos_)
        : _values(index->_values), _size(pos_.size()), _pos(index->iloc_id_(pos_)) {}
    // Index(std::shared_ptr<Index> &index) : {}

    size_t size() const { return _size; }
    void resize(size_t n) { _size = n; }

    virtual std::string name() const { return _values->name(); }
    IndexDType dtype() const { return _dtype; }
    std::string dtype_name() const { return _dtype_name; }

    // Subscripts.
    virtual size_t iloc_id(long long i) const { return _pos[i > 0 ? i : _size - i]; }
    virtual std::vector<size_t> iloc_id(const std::vector<long long> &pos);

    virtual size_t iloc_id_(size_t i) const { return _pos[i]; } // Only for positive index.
    virtual std::vector<size_t> iloc_id_(const std::vector<size_t> &pos);

    virtual std::string iloc_str(long long i) const { return _values->iloc_str(i); }

    // Key-position map.
    virtual size_t loc(long long key) const { throw std::logic_error{"Not implemented."}; }
    virtual size_t loc(const std::string &key) const { throw std::logic_error{"Not implemented."}; }

    virtual void push_back() { ++_size; }

    virtual std::shared_ptr<Index> clone() const;

  protected:
    friend DataFrameView;
    friend DataFrame;

    std::shared_ptr<Series> _values;
    size_t _size = 0;

    std::vector<size_t> _pos; // position in _values.

    IndexDType _dtype = IndexDType::Positional;
    std::string _dtype_name = "Positional";
};

class Int64RangeIndex : public Index {
  public:
    Int64RangeIndex() {}
    explicit Int64RangeIndex(size_t sz) : Index(sz) {
        _end = _start + sz;
        set_info_();
    }

    size_t iloc_id(long long i) const override { return i >= 0 ? i : _end + i; }
    size_t iloc_id_(size_t i) const override { return _start + i; } // Only for positive index.
    // std::string iloc_str(long long i) const override { return _values.iloc_str(i); }

    size_t loc(long long key) const { return key - _start; }

  private:
    long long _start = 0; // start position in data.
    long long _end = 0;   // not included.

    void set_info_() {
        _dtype = IndexDType::Int64Range;
        _dtype_name = "Int64Range";
    }
};

class Int64PeriodIndex : public Index {
  public:
    size_t iloc_id_(size_t i) const override {
        return _start + i * _diff;
    } // Only for positive index.
    size_t iloc_id(long long i) const override { return i > 0 ? iloc_id_(i) : _end + i * _diff; }
    std::string iloc_str(long long i) const override { return _values->iloc_str(i); }

    // size_t loc(long long key) const override { return key - _start; }

  private:
    long long _start = 0, _diff = 0, _end = 0; // end_is the value of last index +_diff
};
}; // namespace dfc
