#pragma once

#include "Series.hpp"

namespace dfc {

class ViewIndex;
class SeriesView;
class DataFrameView;

// Index is always contingous.
// It may have different types of data. So use Series to store data.
class Index {
    friend ViewIndex;
    friend SeriesView;

public:
    // Constructor
    Index() {
    }

    Index(Series &&_values);
    Index(const Index &new_index) = default;
    Index(const ViewIndex &view);

    // info
    IndexDType dtype() const;
    std::string dtype_name() const;
    size_t size() const;
    std::string name() const;
    bool is_trival() const;

    // Subscripts
    // Convert to string. Better for cout.
    template <iloc_type T> size_t iloc(T i) const;
    // size_t iloc(size_t i) const;
    std::string iloc_str(long long n) const;

    // Modification
    void push_back(); // For a trival index, just increase size 1.

    void resize(size_t n);
    void reset_index(long long start = 0); // Reset to trival index.

    IndexType _key_map; // Map key to row index. Default to be monostate
    Series _values;

    size_t _cal_index(long long i) const;

private:
    // size_t _start = 0;
    size_t _size = 0;

    template <typename T> void _create_key_map();
};

class ViewIndex {
public:
    ViewIndex() {
    }

    explicit ViewIndex(size_t n);
    explicit ViewIndex(const std::vector<size_t> &v);
    explicit ViewIndex(std::shared_ptr<Index> index);
    ViewIndex(std::shared_ptr<Index> index,
              const std::vector<long long> &v); // Can contains negative integer.
    ViewIndex(std::shared_ptr<Index> index, const std::vector<size_t> &v);
    // Here pos is the pos in current viewindex.
    template <iloc_type T> ViewIndex(const ViewIndex &view, const std::vector<T> &pos);
    // template <typename T> ViewIndex(const ViewIndex &view, const std::vector<T> &pos);

    friend SeriesView;
    friend DataFrameView;
    friend Index;

    // info
    std::string name() const;
    bool is_trival() const;
    ViewIndexType type() const;
    std::string type_name() const;
    IndexDType dtype() const;
    std::string dtype_name() const;
    size_t size() const;
    bool is_range() const;
    bool is_slice() const;
    template <typename T> std::vector<size_t> get_positive_index(const std::vector<T> &pos);

    // Subscripts.
    size_t
    iloc_id_(size_t i) const; // Only for positive index. Return the position in the original data.
    size_t iloc_id(long long i) const;
    std::string iloc_str(long long n) const;

    // Modification
    void set_index(std::shared_ptr<Index> index);
    size_t _cal_index(long long i) const;
    void reset_index(long long start = 0);

private:
    std::shared_ptr<Index> _index;
    IndexType _key_map;
    // Store discrete integers position.

    // If if range index, _pos[0] is start.
    // If _index_type==Slice, _pos[0] is start, _pos[1] is interval
    // If POS, then _pos[i] is the integer index in _index.
    std::vector<size_t> _pos;
    // Slice _slice;
    size_t _size = 0;
    // bool _is_range = true;
    // bool _is_slice = false;
    ViewIndexType _index_type{ViewIndexType::RANGE};

    void _init_nonrange_index();
};

// str  uct ViewIndexIterator {
//     ViewIndexIterator(ViewIndex *ptr = nullptr) : data(ptr){};
//
//     ViewIndexIterator &operator++();
//
//     ViewIndex *data = nullptr;
// };

}; // namespace dfc
