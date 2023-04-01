#pragma once
#include<vector>

namespace dfc {
template <typename T> std::vector<T> where(const std::vector<bool> &v) {
    std::vector<T> res;
    for (int i = 0; i < v.size(); ++i) {
        if (v[i])
            res.emplace_back(i);
    }
    return res;
}

inline static std::vector<std::string> vec_ccharp_to_vec_str(const std::vector<const char *> v) {
    std::vector<std::string> res;
    for (auto &e : v) {
        res.emplace_back(e);
    }
    return res;

}
}
