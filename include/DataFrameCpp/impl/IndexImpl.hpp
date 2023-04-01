#pragma once

#include "../Index.hpp"

namespace dfc {

inline std::vector<size_t> dfc::Index::iloc_id_(const std::vector<size_t> &pos) {
    std::vector<size_t> res(pos.size());
    std::transform(pos.begin(), pos.end(), res.begin(),
                   [this](const size_t &i) { return _pos[i]; });
    return res;
}

inline std::vector<size_t> dfc::Index::iloc_id(const std::vector<long long> &pos) {
    std::vector<size_t> res(pos.size());
    std::transform(pos.begin(), pos.end(), res.begin(),
                   [this](const size_t &i) { return iloc_id(i); });
    return res;
}

inline std::shared_ptr<Index> dfc::Index::clone() const {
    Index res;
    std::vector<long long> dt(_size);
    std::transform(_pos.begin(), _pos.end(), dt.begin(),
                   [](const size_t &e) { return static_cast<long long>(e); });
    res._values = std::make_shared<Series>(std::move(dt));

    res._size = _size;
    res._pos.resize(_size);
    std::iota(res._pos.begin(), res._pos.end(), 0);
    return std::make_shared<Index>(std::move(res));
}
} // namespace dfc
