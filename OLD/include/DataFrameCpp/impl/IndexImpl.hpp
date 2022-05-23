#pragma once

#include "../Index.hpp"

namespace dfc {
dfc::ViewIndex::ViewIndex(Index &index, const std::vector<long long> &v)
    : _index(&index), _is_range(false), _is_slice(false) {
    _size = v.size();
    for (auto &ele : v) {
        _pos.emplace_back(ele >= 0 ? ele : index.size() + ele);
        std::cout << "pos in view index: " << _pos.back() << std::endl;
    }
}

std::string ViewIndex::name() const { return _index->name(); }
bool ViewIndex::is_trival() const { return _is_range; }
dfc::ViewIndexType dfc::ViewIndex::type() const { return _index_type; }
std::string dfc::ViewIndex::type_name() const {
    if (_is_range)
        return "range";
    else if (_is_slice)
        return _slice.str();
    else
        return "";
}
IndexDType ViewIndex::dtype() const { return _index->dtype(); }
std::string ViewIndex::dtype_name() const { return _index->dtype_name(); }
size_t dfc::ViewIndex::size() const { return _size; }

size_t dfc::ViewIndex::_cal_index(long long i) const { return i >= 0 ? i : _index->size() + i; }

size_t dfc::ViewIndex::iloc_id_(size_t i) const {
    size_t res = 0;
    switch (_index_type) {
    case ViewIndexType::RANGE:
        res = _slice.start + i;
        break;
    case ViewIndexType::SLICE:
        res = _slice.start + _slice.step * i;
        break;
    case ViewIndexType::NORMAL:
        res = _pos[i];
    }
    return res;
}
size_t dfc::ViewIndex::iloc_id(long long i) const { return iloc_id_(i >= 0 ? i : _size + i); }

std::string ViewIndex::iloc_str(long long n) const { return _index->iloc_str(iloc_id_(n)); }

template <typename T> void Index::_create_key_map() {
    std::unordered_map<T, size_t> _key_map;
    auto &values = _values.vector<T>();
    for (int i = 0; i < _size; ++i) {
        auto &key = values[i];
        if (_key_map.contains(key)) {
            auto info = std::format("[Index]: Repeated key: {}. Failed to create index.", key);
            std::cerr << info << std::endl;
            throw(std::runtime_error(info));
        } else {
            _key_map[key] = i;
        }
    }
    this->_key_map = std::move(_key_map);
}

Index::Index(Series &&_values) : _values(std::move(_values)) {
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

IndexDType Index::dtype() const { return static_cast<IndexDType>(_key_map.index()); }
std::string Index::dtype_name() const { return IndexDTypeName[dtype()]; }
size_t Index::size() { return _size; }
std::string Index::name() const { return _values.name(); }
bool Index::is_trival() const { return dtype() == IndexDType::TRIVALINDEX; }

std::string Index::iloc_str(long long n) const {
    auto _n = n;
    if (n < 0) {
        _n = _size + n;
    }

    std::ostringstream res;
    if (dtype() == IndexDType::TRIVALINDEX) {
        res << _n;
    } else if (dtype() == IndexDType::INTINDEX) {
        res << _values.vector<int>()[_n];
    } else if (dtype() == IndexDType::STRINGINDEX) {
        res << _values.vector<std::string>()[_n];
    }
    return res.str();
};

void Index::push_back() {
    if (_key_map.index() == 0) {
        ++_size;
    } else {
        std::cerr << std::format("[Index]: You can't push_back a non-trival index.") << std::endl;
    }
}
void Index::resize(size_t n) {
    if (_key_map.index() == 0) {
        _size = n;
    } else {
        std::cerr << std::format("[Index]: You can't resize a non-trival index.") << std::endl;
    }
}

size_t dfc::Index::_cal_index(long long i) const { return i >= 0 ? i : _size + i; }

ViewIndex::ViewIndex(Index &index) {
    set_index(index);

    if (!index.is_trival())
        _init_nonrange_index();
}

void ViewIndex::_init_nonrange_index() {
    _is_range = false;
    _size = _index->size();
    _pos.resize(_size);
    std::iota(_pos.begin(), _pos.end(), 0);
}

void ViewIndex::set_index(Index &index) { _index = &index; }

bool dfc::ViewIndex::is_range() const { return _is_range; }
bool dfc::ViewIndex::is_slice() const { return _is_slice; }
} // namespace dfc