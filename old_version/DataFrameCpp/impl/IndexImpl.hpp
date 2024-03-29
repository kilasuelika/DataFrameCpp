#pragma once

#include "../Index.hpp"

namespace dfc {
inline dfc::ViewIndex::ViewIndex(std::shared_ptr<Index> index, const std::vector<long long> &v)
    : _index(index), _size(v.size()), _pos(get_positive_index(v)), _index_type(ViewIndexType::POS) {
}

inline dfc::ViewIndex::ViewIndex(std::shared_ptr<Index> index, const std::vector<size_t> &v)
    : _index(index), _size(v.size()), _pos(v), _index_type(ViewIndexType::POS) {
}

inline ViewIndex::ViewIndex(std::shared_ptr<Index> index) { set_index(index); }

template <iloc_type T>
inline dfc::ViewIndex::ViewIndex(const ViewIndex &view, const std::vector<T> &pos)
    : _index(view._index), _size(pos.size()), _pos(pos.size()), _index_type(ViewIndexType::POS) {
    std::transform(std::execution::par_unseq, pos.begin(), pos.end(), _pos.begin(),
                   [this](T i) { return _index->iloc(i); });
}

inline std::string ViewIndex::name() const { return _index->name(); }
inline dfc::ViewIndexType dfc::ViewIndex::type() const { return _index_type; }

inline std::string dfc::ViewIndex::type_name() const {
    switch (_index_type) {
    case ViewIndexType::RANGE:
        return "range";
    case ViewIndexType::SLICE:
        return "slice";
    case ViewIndexType::POS:
        return "pos";
    default:
        std::terminate();
        return "";
    }
}

inline IndexDType ViewIndex::dtype() const { return _index->dtype(); }
inline std::string ViewIndex::dtype_name() const { return _index->dtype_name(); }
inline size_t dfc::ViewIndex::size() const { return _size; }

inline size_t dfc::ViewIndex::_cal_index(long long i) const {
    return i >= 0 ? i : _index->size() + i;
}

inline size_t dfc::ViewIndex::iloc_id_(size_t i) const {
    size_t res = 0;
    switch (_index_type) {
    case ViewIndexType::RANGE:
        res = _pos[0] + i;
        break;
    case ViewIndexType::SLICE:
        res = _pos[0] + _pos[1] * i;
        break;
    case ViewIndexType::POS:
        res = _pos[i];
    }
    return res;
}

inline size_t dfc::ViewIndex::iloc_id(long long i) const {
    return iloc_id_(i >= 0 ? i : _size + i);
}

inline std::string ViewIndex::iloc_str(long long n) const { return _index->iloc_str(iloc_id_(n)); }

template <typename T> inline void Index::_create_key_map() {
    std::unordered_map<T, std::vector<size_t>> _key_map;
    auto &values = _values.vector<T>();
    for (int i = 0; i < _size; ++i) {
        auto &key = values[i];
        if (_key_map.contains(key)) {
            auto info = std::format("[Index]: Repeated key: {}. Failed to create index.", key);
            std::cerr << info << std::endl;
            throw(std::runtime_error(info));
        } else {
            _key_map[key].push_back(i);
        }
    }
    this->_key_map = std::move(_key_map);
}

inline Index::Index(Series &&_values)
    : _values(std::move(_values)) {
    if (_values.dtype() == DType::STRING) {
        // string key
        _size = _values.size();
        _create_key_map<std::string>();
    } else if (_values.dtype() == DType::INT) {
        // int key
        _size = _values.size();
        _create_key_map<int>();
    } else {
        auto info = std::format("[Index]: Invalid key type {}.", _values.dtype_name());
        std::cerr << info << std::endl;
        throw(std::range_error(info));
    }
}

inline dfc::Index::Index(const ViewIndex &view)
    : _size(view.size()) {
    if ((view._index == nullptr) || (view._index->is_trival())) {
        std::cout << "index size: " << _size << std::endl;
        auto start = view._pos[0];
        std::unordered_map<int, std::vector<size_t>> res;
        std::vector<int> values;

        switch (view._index_type) {
        case ViewIndexType::RANGE:
            //If viewindex is range, then new index is trival.
            /*if (view._pos[0] != 0) {
                for (int i = 0; i < _size; ++i) {
                    std::cout << i << std::endl;
                    res[start + i].push_back(i);
                    values.push_back(start + i);
                }
            }*/
            _key_map = long long(start);
            break;
        case ViewIndexType::SLICE:
            for (int i = 0; i < _size; ++i) {
                res[start].push_back(i);
                values.push_back(start);
                start += view._pos[1];
            }
            _key_map = res;
            break;
        case ViewIndexType::POS:
            for (int i = 0; i < _size; ++i) {
                values.push_back(view._pos[i]);
                res[view._pos[i]].push_back(i);
            }
            _key_map = res;
            break;
        default:
            std::cout << std::format("Invalid index type.") << std::endl;
            std::terminate();
        }
        _values = std::move(Series(values));
    } else {
        std::cout << "no" << std::endl;
    }
}

inline IndexDType Index::dtype() const { return static_cast<IndexDType>(_key_map.index()); }
inline std::string Index::dtype_name() const { return IndexDTypeName[dtype()]; }
inline size_t Index::size() const { return _size; }
inline std::string Index::name() const { return _values.name(); }
inline bool Index::is_trival() const { return dtype() == IndexDType::TRIVALINDEX; }

template <iloc_type T> size_t dfc::Index::iloc(T i) const {
    if constexpr (std::is_same_v<T, size_t>) {
        if (is_trival()) {
            return std::get<long long>(_key_map) + i;
        } else {
            return i;
        }
    } else {
        size_t _i = _cal_index(i);
        return iloc<size_t>(_i);
    }
}

inline std::string Index::iloc_str(long long n) const {
    auto _n = n;
    if (n < 0) {
        _n = _size + n;
    }

    std::ostringstream res;
    if (dtype() == IndexDType::TRIVALINDEX) {
        res << std::get<long long>(_key_map) + _n;
    } else if (dtype() == IndexDType::INTINDEX) {
        res << _values.vector<int>()[_n];
    } else if (dtype() == IndexDType::STRINGINDEX) {
        res << _values.vector<std::string>()[_n];
    }
    return res.str();
};

inline void Index::push_back() {
    if (_key_map.index() == 0) {
        ++_size;
    } else {
        std::cerr << std::format("[Index]: You can't append_row a non-trival index.") <<
            std::endl;
    }
}

inline void Index::resize(size_t n) {
    if (_key_map.index() == 0) {
        _size = n;
    } else {
        std::cerr << std::format("[Index]: You can't resize a non-trival index.") <<
            std::endl;
    }
}

inline void dfc::Index::reset_index(long long start) { _key_map = start; }

inline size_t dfc::Index::_cal_index(long long i) const { return i >= 0 ? i : _size + i; }

inline dfc::ViewIndex::ViewIndex(size_t n)
    : _size(n) {
}

inline dfc::ViewIndex::ViewIndex(const std::vector<size_t> &v)
    : _pos(v), _size(v.size()), _index_type(ViewIndexType::POS) {
}

inline void ViewIndex::_init_nonrange_index() {
    _size = _index->size();
    _pos.resize(_size);
    std::iota(_pos.begin(), _pos.end(), 0);
}

inline void ViewIndex::set_index(std::shared_ptr<Index> index) {
    _index = index;
    _size = index->size();

    _index_type = ViewIndexType::RANGE;
    _pos = {0};
}

template <typename T>
std::vector<size_t> dfc::ViewIndex::get_positive_index(const std::vector<T> &pos) {
    std::vector<size_t> res(pos.size());
    std::transform(std::execution::par, pos.cbegin(), pos.cend(), res.begin(),
                   [sz(_size)](const auto &e) { return e >= 0 ? e : sz + e; });
    return res;
}
} // namespace dfc
