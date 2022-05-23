#pragma once

#include "Series.hpp"

namespace dfc {

class ViewIndex;

// Index is always contingous.
class Index {
    friend ViewIndex;

  public:
    // Constructor
    Index() {}
    Index(Series &&_values);
    Index(const Index &new_index) = default;

    // info
    IndexDType dtype() const;
    std::string dtype_name() const;
    size_t size();
    std::string name() const;
    bool is_trival() const;

    // Subscripts
    // Convert to string. Better for cout.
    std::string iloc_str(long long n) const;

    // Modification
    void push_back(); // For a trival index, just increase size 1.

    void resize(size_t n);

    IndexType _key_map; // Map key to row index.
    Series _values;

    size_t _cal_index(long long i) const;

  private:
    // size_t _start = 0;
    size_t _size = 0;

    template <typename T> void _create_key_map();
};

class ViewIndex {
  public:
    ViewIndex() {}
    ViewIndex(size_t n);
    ViewIndex(const std::vector<size_t> &v);
    ViewIndex(Index &index);
    ViewIndex(Index &index, const std::vector<long long> &v); // Can contains negative integer.

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
    void set_index(Index &index);
    size_t _cal_index(long long i) const;

  private:
    Index *_index = nullptr;
    // Store discrete integers position.
    std::vector<size_t> _pos;
    Slice _slice;
    size_t _size = 0;
    // If if range index, then use a start and size to specify range. start stores iloc_id_ _pos[0].
    bool _is_range = true;
    bool _is_slice = false;
    ViewIndexType _index_type{ViewIndexType::RANGE};

    void _init_nonrange_index();
};

// struct ViewIndexIterator {
//     ViewIndexIterator(ViewIndex *ptr = nullptr) : data(ptr){};
//
//     ViewIndexIterator &operator++();
//
//     ViewIndex *data = nullptr;
// };

}; // namespace dfc