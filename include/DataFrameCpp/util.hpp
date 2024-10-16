#pragma once
#include <vector>
#include <map>
#include <set>

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

template <typename MapType>
inline static void erase_map_by_value(MapType &m, const typename MapType::mapped_type &target_value,
                                      int count = 1) {
    int k = 0;
    for (auto it = m.begin(); it != m.end();) {
        if (it->second == target_value) {
            it = m.erase(it);
            ++k;
            if (k > count)
                return;
        } else {
            ++it;
        }
    }
}
template <typename MapType, template <typename> typename SetType = std::set>
inline static void erase_map_by_value(MapType &m,
                                      const SetType<typename MapType::mapped_type> &target_value,
                                      int count = 1) {
    int k = 0;
    for (auto it = m.begin(); it != m.end();) {
        if (target_value.contains(it->second)) {
            it = m.erase(it);
            ++k;
            if (k > count)
                return;
        } else {
            ++it;
        }
    }
}
/**
 * \brief Given a vecotr, remove elements by indices.
 * \tparam ElemType Element type.
 * \param a
 * \param a
 */
template <typename ElemType>
inline static std::vector<int> erase_vec_by_ids(std::vector<ElemType> &v,
                                                std::vector<int> &indices_to_remove) {
    std::sort(indices_to_remove.begin(), indices_to_remove.end());

    // Remove elements at the given indices
    int offset = 0;
    for (const auto &index : indices_to_remove) {
        if (index - offset < v.size()) {
            v.erase(v.begin() + (index - offset));
            ++offset;
        }
    }
    v.erase(indices_to_remove.begin() + offset, indices_to_remove.end());
}
/**
 * \brief Given a set, remove elements by indices.
 * \tparam ElemType Element type.
 * \param a
 * \param a
 */
template <typename ElemType, typename SetType>
inline static std::vector<typename SetType::value_type>
erase_vec_by_ids(std::vector<ElemType> &v, SetType &indices_to_remove_) {
    std::vector<ElemType> indices_to_remove(indices_to_remove_.begin(), indices_to_remove_.end());
    std::sort(indices_to_remove.begin(), indices_to_remove.end());

    // Remove elements at the given indices
    int offset = 0;
    for (const auto &index : indices_to_remove) {
        if (0 <= index - offset && (index - offset) < v.size()) {
            v.erase(v.begin() + (index - offset));
            ++offset;
        }
    }
    v.erase(indices_to_remove.begin() + offset, indices_to_remove.end());
}

template <typename ElemType>
inline static void erase_vec_by_id(std::vector<ElemType> &v, int index_to_remove) {
    v.erase(v.begin() + index_to_remove);
}

inline std::vector<size_t> convert_index(const std::vector<int> &v, int mx) {
    std::vector<size_t> res(v.size());
    std::ranges::transform(res, res.begin(),
                           [mx](const int &e) { return e > 0 ? size_t(e) : size_t(mx + e); });
    return res;
}

template <typename MultiMapType>
MultiMapType::mapped_type get_unique_in_multimap(const MultiMapType &m,
                                                 const typename MultiMapType::key_type &key) {
    auto it = m.equal_range(key);
    return it.first->second;
}
} // namespace dfc
