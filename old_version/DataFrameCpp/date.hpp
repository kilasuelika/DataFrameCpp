#pragma once
#include<string>
#include<boost/date_time.hpp>
#include<execution>
#include<boost/lexical_cast.hpp>

namespace dfc {
template <typename StrT, typename DT> DT from_string(const StrT &s) {
    if constexpr (std::is_same_v<DT, boost::gregorian::date>) {
        if (s.size() >= 10) {
            return DT(boost::lexical_cast<int>(std::string_view(s.data(), 4)),
                      boost::lexical_cast<int>(std::string_view(s.data() + 5, 2)),
                      boost::lexical_cast<int>(std::string_view(s.data() + 8, 2)));
        } else if (s.size() >= 8) {
            return DT(boost::lexical_cast<int>(std::string_view(s.data(), 4)),
                      boost::lexical_cast<int>(std::string_view(s.data() + 4, 2)),
                      boost::lexical_cast<int>(std::string_view(s.data() + 6, 2)));
        } else {
            DT();
        }
    } else if constexpr (std::is_same_v<DT, boost::posix_time::ptime>) {
        return boost::posix_time::time_from_string(s);
    }
}

template <typename StrT, typename DT> std::vector<DT>
from_string_vector(const std::vector<StrT> &v) {
    std::vector<DT> res(v.size());
    std::transform(std::execution::par, v.begin(), v.end(),
                   res.begin(), [](const auto &e) { return from_string<StrT, DT>(e); });
    return res;
}
}

namespace std {

inline std::string to_string(const boost::gregorian::date &dt) {
    return boost::gregorian::to_iso_extended_string(dt);
}

inline std::string to_string(const boost::posix_time::ptime &t) {
    return boost::posix_time::to_iso_extended_string(t);
}

inline std::string to_string(const boost::gregorian::date_duration &dt) {
    std::ostringstream ss;
    ss << dt;
    return ss.str();
}

inline std::string to_string(const boost::posix_time::time_duration &dt) {
    std::ostringstream ss;
    ss << dt;
    return ss.str();
}

} // namespace std
