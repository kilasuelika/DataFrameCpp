#pragma once

// @kilasuelika
// May 19, 2022

#include "../Series.hpp"
#include "../SeriesView.hpp"

namespace dfc {

    inline dfc::Series::Series(size_t n) : _size(n) {}

    template <typename T> inline Series::Series(const std::vector<T>& data) {
        _size = data.size();
        _values = data;
    }

    template <typename T> inline Series::Series(std::vector<T>&& data) {
        _size = data.size();
        _values = std::move(data);
    }

    template <typename T> inline Series::Series(const std::string& name, std::vector<T> data) {
        _size = data.size();
        _name = name;
        _values = std::move(data);
    }

    template <typename T> inline Series::Series(const std::string& name, std::vector<T>&& data) {
        _size = data.size();
        _name = name;
        _values = data;
    }

    // template <typename T>
    // Series::Series(const std::string &name, size_t size)
    //     : _name(name), _size(size), _values(std::vector<T>(size)) {}

    inline Series::Series(const std::string& name, DType type, size_t sz) : _name(name), _size(sz) {
        switch (type) {
        case DType::STRING:
            _values = std::vector<std::string>(sz);
            break;
        case DType::BOOL:
            _values = std::vector<bool>(sz);
            break;
        case DType::INT:
            _values = std::vector<int>(sz);
            break;
        case DType::LONGLONG:
            _values = std::vector<long long>(sz);
            break;
        case DType::FLOAT:
            _values = std::vector<float>(sz);
            break;
        case DType::DOUBLE:
            _values = std::vector<double>(sz);
            break;
        case DType::DATE:
            _values = std::vector<date>(sz);
            break;
        case DType::DATETIME:
            _values = std::vector<ptime>(sz);
            break;
        case DType::DATEDURATION:
            _values = std::vector<date_duration>(sz);
            break;
        case DType::TIMEDURATION:
            _values = std::vector<time_duration>(sz);
            break;
        default:
            std::cout << std::format("Invalid data type {}.", static_cast<int>(type)) << std::endl;
            break;
        }
        fmt::print("new series, size {}\n", sz);
    }

    template <typename T> inline Series::Series(std::initializer_list<T> l) {
        if constexpr (!std::is_same_v<T, const char*>) {
            std::vector<T> data(l);
            _size = data.size();
            _values = std::move(data);
        }
        else {
            std::vector<std::string> data(l.size());
            int i = 0;
            for (auto& ele : l) {
                data[i] = ele;
                ++i;
            }
            _size = i;
            _values = std::move(data);
        }
    }

    template <typename T> inline Series::Series(const std::string& name, std::initializer_list<T> l) {
        _name = name;
        if constexpr (!std::is_same_v<T, const char*>) {
            std::vector<T> data(l);
            _size = data.size();
            _values = std::move(data);
        }
        else {
            std::vector<std::string> data(l.size());
            int i = 0;
            for (auto& e : l) {
                data[i] = e;
                ++i;
            }
            _size = data.size();
            _values = std::move(data);
        }
    }

    inline Series::Series(const Series& new_series)
        : _name(new_series._name), _size(new_series._size), _values(new_series._values) {
    }

    inline Series::Series(Series&& new_series)
        : _name(std::move(new_series._name)), _size(new_series._size),
        _values(std::move(new_series._values)) {
    }

    inline dfc::Series::Series(const SeriesView& view) : _name(view.name()), _size(view.size()) {
        std::visit(
            [&view, this](auto&& v) -> void {
                using VecT = std::decay_t<decltype(v)>;
                if constexpr (not std::is_same_v<VecT, std::monostate>) {
                    using EleT = typename VecT::value_type;
                    VecT res(view.size());
                    for (int i = 0; i < view.size(); ++i) {
                        res[i] = view.iloc_<EleT>(i);
                    }
                    this->_values = res;
                };
            },
            view._values->_values);
    }

    inline Series& Series::operator=(const Series& A) {
        _name = A._name;
        _size = A._size;
        _values = A._values;
        return *this;
    }

    inline const std::string& Series::name() const { return _name; };
    inline void Series::rename(const std::string& name) { _name = name; }
    inline DType Series::dtype() const { return static_cast<DType>(_values.index()); };
    inline std::string Series::dtype_name() const { return DTypeName[_values.index()]; };
    inline size_t Series::size() const { return _size; };
    inline bool Series::empty() const { return _values.index() == 0; }

    template <typename T> inline T& Series::iloc(long long i) {
        return const_cast<T&>(const_cast<const Series*>(this)->iloc<T>(i));
    }

    template <typename T> inline const T& Series::iloc(long long i) const {
        return std::get<std::vector<T>>(_values)[_cal_index(i)];
    }

    template <typename T> inline const T& Series::iloc_(size_t i) const {
        return std::get<std::vector<T>>(_values)[i];
    }

    template <typename T> inline T& Series::iloc_(size_t i) {
        return const_cast<T&>(const_cast<const Series*>(this)->iloc_<T>(i));
    }

    inline std::string Series::iloc_str(long long i) const {
        auto n = _cal_index(i);
        return iloc_str_(n);
    }

    inline std::string Series::iloc_str_(long long i) const {
        // std::string res;
        std::ostringstream res;
        switch (dtype()) {
        case DType::STRING:
            res << iloc<std::string>(i);
            break;
        case DType::BOOL:
            res << iloc<bool>(i);
            break;
        case DType::INT:
            res << iloc<int>(i);
            break;
        case DType::LONGLONG:
            res << iloc<long long>(i);
            break;
        case DType::FLOAT:
            res << iloc<float>(i);
            break;
        case DType::DOUBLE:
            res << iloc<double>(i);
            break;
        case DType::DATE:
            res << std::to_string(iloc<date>(i));
            break;
        case DType::DATETIME:
            res << std::to_string(iloc<ptime>(i));
            break;
        case DType::DATEDURATION:
            res << std::to_string(iloc<date_duration>(i));
            break;
        case DType::TIMEDURATION:
            res << std::to_string(iloc<time_duration>(i));
            break;
        default:
            break;
        }
        return res.str();
    }

    inline dfc::SeriesView dfc::Series::iloc(const std::vector<long long>& pos) {
        return SeriesView(this);
    }

    // Even if _values is in monostate, the result is correct as a new vector res is created.
    template <typename T> inline void Series::astype() {
        if (dtype() == DType::NONE) {
            _values = std::vector<T>();
            return;
        }
        else {
            if constexpr (std::is_same_v<T, std::string>) {
                // Convert current to string.
                if ((_values.index() > 1) && (_values.index() < 7)) {
                    std::vector<std::string> res(_size);
                    std::visit(
                        [&res, this](auto&& v) {
                            using VecT = std::decay_t<decltype(v)>;
                            if constexpr (not std::is_same_v<VecT, std::monostate>) {
                                using EleT = typename VecT::value_type;
                                if constexpr (not std::is_same_v<EleT, std::string>) {
                                    std::transform(
                                        std::execution::par, v.begin(), v.end(), res.begin(),
                                        [](const EleT& e) -> std::string { return std::to_string(e); });
                                }
                            }
                        },
                        _values);
                    _values = res;
                }
            }
            else if constexpr (std::is_arithmetic_v<T>) {
                // Target arithmetic type.
                if (is_arithmetic(_values.index())) {
                    // From another numeric type.
                    std::vector<T> res(_size);
                    std::visit(
                        [&res, this](auto&& v) {
                            using VecT = std::decay_t<decltype(v)>;
                            if constexpr (not std::is_same_v<VecT, std::monostate>) {
                                using EleT = typename VecT::value_type;
                                if constexpr (std::is_arithmetic_v<EleT>) {
                                    std::transform(
                                        std::execution::par, v.begin(), v.end(), res.begin(),
                                        [](const EleT& e) -> T { return static_cast<T>(e); });
                                }
                                else if constexpr (date_time_type<EleT>) {
                                }
                            }
                        },
                        _values);
                    _values = res;
                }
                else if (_values.index() == 1) {
                    // From string.
                    std::vector<T> res(_size);
                    auto& v = std::get<std::vector<std::string>>(_values);
                    std::transform(std::execution::par, v.begin(), v.end(), res.begin(),
                        [](const std::string& e) -> T {
                            try {
                                auto v = boost::lexical_cast<T>(e);
                                return v;
                            }
                            catch (...) {
                                return std::numeric_limits<double>::quiet_NaN();
                            }
                        });
                    _values = res;
                }
                else {
                    // From date_time
                    if (_values.index() == static_cast<size_t>(DType::DATEDURATION)) {
                        std::vector<T> res(_size);
                        auto& v = std::get<std::vector<date_duration>>(_values);
                        std::transform(std::execution::par, v.begin(), v.end(), res.begin(),
                            [](const auto& e) { return static_cast<T>(e.days()); });
                        _values = res;
                    }
                    else {
                        fmt::cerr(
                            "Can't convert date_time type except date_duration to arithmetic type.\n");
                    }
                }
            }
            else {
                // Target datetime.
                if (_values.index() == static_cast<size_t>(DType::STRING)) {
                    // From string.
                    auto& v = std::get<std::vector<std::string>>(_values);
                    if constexpr (std::is_same_v<T, date> || std::is_same_v<T, ptime>) {
                        _values = from_string_vector<std::string, T>(
                            std::get<std::vector<std::string>>(_values));
                    }
                    else {
                    }
                }
                else if (_values.index() == static_cast<size_t>(DType::INT)) {
                    // From int to date_duration.
                    if constexpr (std::is_same_v<T, date_duration>) {
                        std::vector<date_duration> res(_size);
                        auto& v = std::get<std::vector<int>>(_values);
                        std::transform(std::execution::par, v.begin(), v.end(), res.begin(),
                            [](const auto& e) { return date_duration(e); });
                        _values = res;
                    }
                    else if constexpr (std::is_same_v<T, date>) {
                        std::vector<date> res(_size);
                        auto& v = std::get<std::vector<int>>(_values);
                        std::transform(std::execution::par, v.begin(), v.end(), res.begin(),
                            [](const auto& e) {
                                int y = e / 10000;
                                int md = e % 10000;
                                int m = md / 100;
                                int d = md % 100;
                                return date(y, m, d);
                            });
                        _values = res;
                    }
                    else {
                        fmt::cerr("Cant' convert {} to {}.\n", DTypeName[_values.index()],
                            std::type_index(typeid(T)).name());
                    }
                }
            }
        }
    }

    inline void dfc::Series::resize(size_t sz) {
        if (sz == 0)
            return;
        switch (_values.index()) {
        case 0:
            std::cerr << "Column is not set to any type. Can't resize." << std::endl;
            break;
        case 1:
            std::get<std::vector<std::string>>(_values).resize(sz);
            break;
        case 2:
            std::get<std::vector<bool>>(_values).resize(sz);
            break;
        case 3:
            std::get<std::vector<int>>(_values).resize(sz);
            break;
        case 4:
            std::get<std::vector<long long>>(_values).resize(sz);
            break;
        case 5:
            std::get<std::vector<float>>(_values).resize(sz);
            break;
        case 6:
            std::get<std::vector<double>>(_values).resize(sz);
            break;
        default:
            break;
        }
    }

    template <typename T> inline void Series::reserve(size_t sz) {
        if (_values.index() == 0) {
            // Current no data.
            std::vector<T> res;
            res.reserve(sz);
            _values = std::move(res);
        }
        else {
            if (DTypeMap[std::type_index(typeid(T))] == _values.index()) {
                // T and current data type are the same
                std::get<T>(_values).reserve(sz);
            }
            else {
                // Not equal, do nothing.
                std::cerr << "[Series]: You try to reserve " << typeid(T).name()
                    << " elments. But the current data container has " << dtype_name()
                    << " elements. Nothing is done." << std::endl;
            }
        }
    }

    inline void Series::push_back() {
        if (_values.index() != 0) {
            std::visit(
                [this](auto&& v) {
                    using VecT = std::decay_t<decltype(v)>;
                    if constexpr (not std::is_same_v<VecT, std::monostate>) {
                        v.emplace_back();
                    };
                },
                _values);
        }
        ++_size;
    }

    template <typename T> inline void Series::push_back_() {
        std::get<std::vector<T>>(_values).emplace_back();
    }

    template <typename T> inline void Series::push_back(const T& e) {
        std::get<std::vector<T>>(_values).emplace_back(e);
        _size++;
    }

    inline const SeriesType& Series::values() const { return _values; };

    template <typename T> inline std::vector<T> dfc::Series::asvector() const {
        Series new_series = *this;
        new_series.astype<T>();
        return std::move(new_series.vector<T>());
    }

    template <typename T> inline const std::vector<T>& Series::vector() const {
        return std::get<std::vector<T>>(_values);
    };

    template <typename T> inline std::vector<T>& dfc::Series::_vector() {
        return std::get<std::vector<T>>(_values);
    }

    inline size_t Series::_cal_index(long long i) const {
        if (i >= 0) {
            return i;
        }
        else {
            return _size + i;
        }
    }

    inline std::vector<size_t> dfc::Series::_cal_index(const std::vector<long long>& i) const {
        std::vector<size_t> res(i.size());
        std::transform(i.begin(), i.end(), res.begin(), [this](long long j) { return _cal_index(j); });
        return res;
    }

    inline std::ostream& operator<<(std::ostream& os, const Series& df) {
        os << df.name() << std::endl << df.dtype_name() << std::endl;
        for (auto i = 0; i < df._size; ++i) {
            os << df.iloc_str(i) << std::endl;
        }
        os << "Length: " << df.size() << std::flush;
        return os;
    }

    // Series-Series bin op can utilize parallel execution.
#define DataFrameCpp_SERIES_SERIES_BINOP_ASSIGNMENT_IMPL_(OPNAME, transformop)                     \
    template <typename T1, typename T2>                                                            \
    inline void Series::OPNAME##_assignment(const Series &obj) {                                   \
        if (DType_traits<T1>::dtype != dtype()) {                                                  \
            astype<T1>();                                                                          \
        }                                                                                          \
        auto &lhs = _vector<T1>();                                                                 \
        auto &rhs = obj.vector<T2>();                                                              \
        std::transform(std::execution::par, lhs.cbegin(), lhs.cend(), rhs.cbegin(), lhs.begin(),   \
                       std::transformop<T1>{});                                                    \
    }

    DataFrameCpp_SERIES_SERIES_BINOP_ASSIGNMENT_IMPL_(add, plus);
    DataFrameCpp_SERIES_SERIES_BINOP_ASSIGNMENT_IMPL_(sub, minus);
    DataFrameCpp_SERIES_SERIES_BINOP_ASSIGNMENT_IMPL_(mul, multiplies);
    DataFrameCpp_SERIES_SERIES_BINOP_ASSIGNMENT_IMPL_(div, divides);

#include "AssignmentMacro.hpp"
    DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(Series, SeriesView, +=, add);
    DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(Series, SeriesView, -=, sub);
    DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(Series, SeriesView, *=, mul);
    DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(Series, SeriesView, /=, div);

    DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(Series, Series, +=, add_assignment);
    DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(Series, Series, -=, sub_assignment);
    DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(Series, Series, *=, mul_assignment);
    DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(Series, Series, /=, div_assignment);

    DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(Series, SeriesView, +=, add_assignment);
    DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(Series, SeriesView, -=, sub_assignment);
    DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(Series, SeriesView, *=, mul_assignment);
    DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(Series, SeriesView, /=, div_assignment);

    DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(Series, SeriesView, add, +);
    DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(Series, SeriesView, sub, -);
    DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(Series, SeriesView, mul, *);
    DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(Series, SeriesView, div, / );

    DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(Series, Series, add, +);
    DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(Series, Series, sub, -);
    DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(Series, Series, mul, *);
    DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(Series, Series, div, / );

    DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(Series, SeriesView, add, +);
    DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(Series, SeriesView, sub, -);
    DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(Series, SeriesView, mul, *);
    DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(Series, SeriesView, div, / );

    DataFrameCpp_SERIES_SERIES_ARITH_BIN_OP_IMPL_(+, add_assignment);
    DataFrameCpp_SERIES_SERIES_ARITH_BIN_OP_IMPL_(-, sub_assignment);
    DataFrameCpp_SERIES_SERIES_ARITH_BIN_OP_IMPL_(*, mul_assignment);
    DataFrameCpp_SERIES_SERIES_ARITH_BIN_OP_IMPL_(/ , div_assignment);

#include "UndefMacro.hpp"

#define DataFrameCpp_Series_Series_Compare_Transform(type, op)                                     \
    auto &lhs_v = lhs.vector<type>();                                                              \
    auto &rhs_v = rhs.vector<type>();                                                              \
    std::transform(std::execution::par, lhs_v.cbegin(), lhs_v.cend(), rhs_v.cbegin(),              \
                   res_v.begin(), [](const type &e1, const type &e2) -> bool { return e1 op e2; })

#define DataFrameCpp_Series_Series_Compare_Case_level2(ltype, dtypename, type, op)                 \
    case DType::dtypename: {                                                                       \
        auto &rhs_v = rhs.vector<type>();                                                          \
        std::transform(lhs_v.cbegin(), lhs_v.cend(), rhs_v.cbegin(), res_v.begin(),                \
                       [](const ltype &e1, const type &e2) -> bool { return e1 op e2; });          \
        break;                                                                                     \
    }
#define DataFrameCpp_Series_Series_Compare_Case_level1(dtypename, type, op)                        \
    case DType::dtypename: {                                                                       \
        auto &lhs_v = lhs.vector<type>();                                                          \
        switch (rhs.dtype()) {                                                                     \
            DataFrameCpp_Series_Series_Compare_Case_level2(type, BOOL, bool, op);                  \
            DataFrameCpp_Series_Series_Compare_Case_level2(type, INT, int, op);                    \
            DataFrameCpp_Series_Series_Compare_Case_level2(type, LONGLONG, long long, op);         \
            DataFrameCpp_Series_Series_Compare_Case_level2(type, FLOAT, float, op);                \
            DataFrameCpp_Series_Series_Compare_Case_level2(type, DOUBLE, double, op);              \
        default:                                                                                   \
            break;                                                                                 \
        };                                                                                         \
        break;                                                                                     \
    }

#define DataFrameCpp_Series_Series_Compare(op)                                                     \
    inline Series dfc::operator op(const Series &lhs, const Series &rhs) {                         \
        if (lhs.size() != rhs.size()) {                                                            \
            fmt::cerr("Size of lhs {} and rhs {} is not equal.\n", lhs.name(), rhs.name());        \
            std::abort();                                                                          \
        }                                                                                          \
        Series res("", DType::BOOL, lhs.size());                                                   \
        auto &res_v = res._vector<bool>();                                                         \
        if (is_arithmetic(lhs.dtype()) && is_arithmetic(rhs.dtype())) {                            \
            switch (lhs.dtype()) {                                                                 \
                DataFrameCpp_Series_Series_Compare_Case_level1(BOOL, bool, op);                    \
                DataFrameCpp_Series_Series_Compare_Case_level1(INT, int, op);                      \
                DataFrameCpp_Series_Series_Compare_Case_level1(LONGLONG, long long, op);           \
                DataFrameCpp_Series_Series_Compare_Case_level1(FLOAT, float, op);                  \
                DataFrameCpp_Series_Series_Compare_Case_level1(DOUBLE, double, op);                \
            default:                                                                               \
                break;                                                                             \
            }                                                                                      \
        } else if ((lhs.dtype() == DType::DATE) && (rhs.dtype() == DType::DATE)) {                 \
            DataFrameCpp_Series_Series_Compare_Transform(date, op);                                \
        } else if ((lhs.dtype() == DType::DATETIME) && (rhs.dtype() == DType::DATETIME)) {         \
            DataFrameCpp_Series_Series_Compare_Transform(ptime, op);                               \
        } else if ((lhs.dtype() == DType::DATEDURATION) && (rhs.dtype() == DType::DATEDURATION)) { \
            DataFrameCpp_Series_Series_Compare_Transform(date_duration, op);                       \
        } else if ((lhs.dtype() == DType::TIMEDURATION) && (rhs.dtype() == DType::TIMEDURATION)) { \
            DataFrameCpp_Series_Series_Compare_Transform(time_duration, op);                       \
        } else {                                                                                   \
            std::string info =                                                                     \
                std::format("Can't compare between {}({}) and {}({}).\n", lhs.name(),              \
                            lhs.dtype_name(), rhs.name(), rhs.dtype_name());                       \
                                                                                   \
            throw(std::runtime_error(info));                                                       \
        }                                                                                          \
        return res;                                                                                \
    }

    DataFrameCpp_Series_Series_Compare(< );
    DataFrameCpp_Series_Series_Compare(> );
    DataFrameCpp_Series_Series_Compare(<= );
    DataFrameCpp_Series_Series_Compare(>= );
    DataFrameCpp_Series_Series_Compare(== );
    DataFrameCpp_Series_Series_Compare(!= );

#define DataFrameCpp_Series_Series_Bitwise_Bin_Op(op)                                              \
    inline Series operator op(const Series &lhs, const Series &rhs) {                              \
        if (lhs.size() != rhs.size()) {                                                            \
            fmt::cerr("Size of lhs {} and rhs {} is not equal.\n", lhs.name(), rhs.name());        \
            std::abort();                                                                          \
        }                                                                                          \
        Series res("", DType::BOOL, lhs.size());                                                   \
        auto &res_v = res._vector<bool>();                                                         \
        if (is_arithmetic(lhs.dtype()) && is_arithmetic(rhs.dtype())) {                            \
            switch (lhs.dtype()) {                                                                 \
                DataFrameCpp_Series_Series_Compare_Case_level1(BOOL, bool, op);                    \
                DataFrameCpp_Series_Series_Compare_Case_level1(INT, int, op);                      \
                DataFrameCpp_Series_Series_Compare_Case_level1(LONGLONG, long long, op);           \
                DataFrameCpp_Series_Series_Compare_Case_level1(FLOAT, float, op);                  \
                DataFrameCpp_Series_Series_Compare_Case_level1(DOUBLE, double, op);                \
            default:                                                                               \
                break;                                                                             \
            }                                                                                      \
        } else {                                                                                   \
            std::string info =                                                                     \
                std::format("Can't run bitwise operations on {}({}) and {}({}).\n", lhs.name(),    \
                            lhs.dtype_name(), rhs.name(), rhs.dtype_name());                       \
            fmt::cerr(info);                                                                       \
            throw(std::runtime_error(info));                                                       \
        }                                                                                          \
        return res;                                                                                \
    }
    DataFrameCpp_Series_Series_Bitwise_Bin_Op(&&);
    DataFrameCpp_Series_Series_Bitwise_Bin_Op(|| );

#undef DataFrameCpp_Series_Series_Compare
#undef DataFrameCpp_Series_Series_Compare_Case_level1
#undef DataFrameCpp_Series_Series_Compare_Case_level2
#undef DataFrameCpp_Series_Series_Compare_Transform
#undef DataFrameCpp_Series_Series_Bitwise_Bin_Op
#undef DataFrameCpp_Series_Series_Bitwise_Bin_Op
    //--------------------------------------------------------------
    // Series SeriesView

#define DataFrameCpp_Series_SeriesView_Compare_Transform(type, op)                                 \
    auto &lhs_v = lhs.vector<type>();                                                              \
    _Pragma("omp parallel for") for (int i = 0; i < lhs.size(); ++i) {                             \
        res_v[i] = lhs_v[i] op rhs.iloc_<type>(i);                                                 \
    }

#define DataFrameCpp_Series_SeriesView_Compare_Case_level2(ltype, dtypename, type, op)             \
    case DType::dtypename: {                                                                       \
        _Pragma("omp parallel for") for (int i = 0; i < lhs.size(); ++i) {                         \
            res_v[i] = lhs_v[i] op rhs.iloc_<type>(i);                                             \
        }                                                                                          \
        break;                                                                                     \
    }
#define DataFrameCpp_Series_SeriesView_Compare_Case_level1(dtypename, type, op)                    \
    case DType::dtypename: {                                                                       \
        auto &lhs_v = lhs.vector<type>();                                                          \
        switch (rhs.dtype()) {                                                                     \
            DataFrameCpp_Series_SeriesView_Compare_Case_level2(type, BOOL, bool, op);              \
            DataFrameCpp_Series_SeriesView_Compare_Case_level2(type, INT, int, op);                \
            DataFrameCpp_Series_SeriesView_Compare_Case_level2(type, LONGLONG, long long, op);     \
            DataFrameCpp_Series_SeriesView_Compare_Case_level2(type, FLOAT, float, op);            \
            DataFrameCpp_Series_SeriesView_Compare_Case_level2(type, DOUBLE, double, op);          \
        default:                                                                                   \
            break;                                                                                 \
        };                                                                                         \
        break;                                                                                     \
    }

#define DataFrameCpp_Series_SeriesView_Compare(op)                                                 \
    inline Series dfc::operator op(const Series &lhs, const SeriesView &rhs) {                     \
        if (lhs.size() != rhs.size()) {                                                            \
            fmt::cerr("Size of lhs {} and rhs {} is not equal.\n", lhs.name(), rhs.name());        \
            std::abort();                                                                          \
        }                                                                                          \
        Series res("", DType::BOOL, lhs.size());                                                   \
        auto &res_v = res._vector<bool>();                                                         \
        if (is_arithmetic(lhs.dtype()) && is_arithmetic(rhs.dtype())) {                            \
            switch (lhs.dtype()) {                                                                 \
                DataFrameCpp_Series_SeriesView_Compare_Case_level1(BOOL, bool, op);                \
                DataFrameCpp_Series_SeriesView_Compare_Case_level1(INT, int, op);                  \
                DataFrameCpp_Series_SeriesView_Compare_Case_level1(LONGLONG, long long, op);       \
                DataFrameCpp_Series_SeriesView_Compare_Case_level1(FLOAT, float, op);              \
                DataFrameCpp_Series_SeriesView_Compare_Case_level1(DOUBLE, double, op);            \
            default:                                                                               \
                break;                                                                             \
            }                                                                                      \
        } else if ((lhs.dtype() == DType::DATE) && (rhs.dtype() == DType::DATE)) {                 \
            DataFrameCpp_Series_SeriesView_Compare_Transform(date, op);                            \
        } else if ((lhs.dtype() == DType::DATETIME) && (rhs.dtype() == DType::DATETIME)) {         \
            DataFrameCpp_Series_SeriesView_Compare_Transform(ptime, op);                           \
        } else if ((lhs.dtype() == DType::DATEDURATION) && (rhs.dtype() == DType::DATEDURATION)) { \
            DataFrameCpp_Series_SeriesView_Compare_Transform(date_duration, op);                   \
        } else if ((lhs.dtype() == DType::TIMEDURATION) && (rhs.dtype() == DType::TIMEDURATION)) { \
            DataFrameCpp_Series_SeriesView_Compare_Transform(time_duration, op);                   \
        } else {                                                                                   \
            std::string info =                                                                     \
                std::format("Can't compare between {}({}) and {}({}).\n", lhs.name(),              \
                            lhs.dtype_name(), rhs.name(), rhs.dtype_name());                       \
            fmt::cerr(info);                                                                       \
            throw(std::runtime_error(info));                                                       \
        }                                                                                          \
        return res;                                                                                \
    }                                                                                              \
    inline Series operator op(const SeriesView &lhs, const Series &rhs) { return rhs op lhs; }

    DataFrameCpp_Series_SeriesView_Compare(< );
    DataFrameCpp_Series_SeriesView_Compare(> );
    DataFrameCpp_Series_SeriesView_Compare(<= );
    DataFrameCpp_Series_SeriesView_Compare(>= );
    DataFrameCpp_Series_SeriesView_Compare(== );
    DataFrameCpp_Series_SeriesView_Compare(!= );

#define DataFrameCpp_Series_SeriesView_Bitwise_Bin_Op(op)                                          \
    inline Series operator op(const Series &lhs, const SeriesView &rhs) {                          \
        if (lhs.size() != rhs.size()) {                                                            \
            fmt::cerr("Size of lhs {} and rhs {} is not equal.\n", lhs.name(), rhs.name());        \
            std::abort();                                                                          \
        }                                                                                          \
        Series res("", DType::BOOL, lhs.size());                                                   \
        auto &res_v = res._vector<bool>();                                                         \
        if (is_arithmetic(lhs.dtype()) && is_arithmetic(rhs.dtype())) {                            \
            switch (lhs.dtype()) {                                                                 \
                DataFrameCpp_Series_SeriesView_Compare_Case_level1(BOOL, bool, op);                \
                DataFrameCpp_Series_SeriesView_Compare_Case_level1(INT, int, op);                  \
                DataFrameCpp_Series_SeriesView_Compare_Case_level1(LONGLONG, long long, op);       \
                DataFrameCpp_Series_SeriesView_Compare_Case_level1(FLOAT, float, op);              \
                DataFrameCpp_Series_SeriesView_Compare_Case_level1(DOUBLE, double, op);            \
            default:                                                                               \
                break;                                                                             \
            }                                                                                      \
        } else {                                                                                   \
            std::string info =                                                                     \
                std::format("Can't run bitwise operations on {}({}) and {}({}).\n", lhs.name(),    \
                            lhs.dtype_name(), rhs.name(), rhs.dtype_name());                       \
            fmt::cerr(info);                                                                       \
            throw(std::runtime_error(info));                                                       \
        }                                                                                          \
        return res;                                                                                \
    }                                                                                              \
    inline Series operator op(const SeriesView &lhs, const Series &rhs) { return rhs op lhs; }
    DataFrameCpp_Series_SeriesView_Bitwise_Bin_Op(&&);
    DataFrameCpp_Series_SeriesView_Bitwise_Bin_Op(|| );

#undef DataFrameCpp_Series_SeriesView_Compare
#undef DataFrameCpp_Series_SeriesView_Compare_Case_level1
#undef DataFrameCpp_Series_SeriesView_Compare_Case_level2
#undef DataFrameCpp_Series_SeriesView_Compare_Transform

    //-----------------------------------------------------------------
    // Series scalar
#define DataFrameCpp_Series_Scalar_Compare_Transform(type, op)                                     \
    auto &lhs_v = lhs.vector<type>();                                                              \
    std::transform(std::execution::par, lhs_v.cbegin(), lhs_v.cend(), res_v.begin(),               \
                   [&](const type &l) { return l op rhs; })

#define DataFrameCpp_Series_Scalar_Compare_Case_level1(dtypename, type, op)                        \
    case DType::dtypename: {                                                                       \
        DataFrameCpp_Series_Scalar_Compare_Transform(type, op);                                    \
        break;                                                                                     \
    }

#define DataFrameCpp_Series_Scalar_Compare(op)                                                     \
    template <supported_type S> inline Series operator op(const Series &lhs, const S &rhs) {       \
        const auto rdtype = DType_traits<S>::dtype;                                                \
        Series res("", DType::BOOL, lhs.size());                                                   \
        auto &res_v = res._vector<bool>();                                                         \
        if constexpr (arithmetic_type<S>) {                                                        \
            switch (lhs.dtype()) {                                                                 \
                DataFrameCpp_Series_Scalar_Compare_Case_level1(BOOL, bool, op);                    \
                DataFrameCpp_Series_Scalar_Compare_Case_level1(INT, int, op);                      \
                DataFrameCpp_Series_Scalar_Compare_Case_level1(LONGLONG, long long, op);           \
                DataFrameCpp_Series_Scalar_Compare_Case_level1(FLOAT, float, op);                  \
                DataFrameCpp_Series_Scalar_Compare_Case_level1(DOUBLE, double, op);                \
            default:                                                                               \
                break;                                                                             \
            }                                                                                      \
        } else if constexpr (std::is_same_v<S, date>) {                                            \
            if (lhs.dtype() == DType::DATE) {                                                      \
                DataFrameCpp_Series_Scalar_Compare_Transform(date, op);                            \
            }                                                                                      \
        } else if constexpr (std::is_same_v<S, ptime>) {                                           \
            if (lhs.dtype() == DType::DATETIME) {                                                  \
                DataFrameCpp_Series_Scalar_Compare_Transform(ptime, op);                           \
            }                                                                                      \
        } else if constexpr (std::is_same_v<S, date_duration>) {                                   \
            if (lhs.dtype() == DType::DATEDURATION) {                                              \
                DataFrameCpp_Series_Scalar_Compare_Transform(date_duration, op);                   \
            }                                                                                      \
        } else if constexpr (std::is_same_v<S, time_duration>) {                                   \
            if (lhs.dtype() == DType::TIMEDURATION) {                                              \
                DataFrameCpp_Series_Scalar_Compare_Transform(time_duration, op);                   \
            }                                                                                      \
        } else {                                                                                   \
            std::string info = std::format("Can't compare between {}({}) and scalar type().\n",    \
                                           lhs.name(), lhs.dtype_name(), DType_traits<S>::name);   \
            fmt::cerr(info);                                                                       \
            throw(std::runtime_error(info));                                                       \
        }                                                                                          \
        return res;                                                                                \
    }                                                                                              \
    template <supported_type S> inline Series operator op(const S &rhs, const Series &lhs) {       \
        return lhs op rhs;                                                                         \
    }

    DataFrameCpp_Series_Scalar_Compare(< );
    DataFrameCpp_Series_Scalar_Compare(> );
    DataFrameCpp_Series_Scalar_Compare(<= );
    DataFrameCpp_Series_Scalar_Compare(>= );
    DataFrameCpp_Series_Scalar_Compare(== );
    DataFrameCpp_Series_Scalar_Compare(!= );

#define DataFrameCpp_Series_Scalar_Bitwise_Bin_Op(op)                                              \
    template <supported_type S> inline Series operator op(const Series &lhs, const S &rhs) {       \
        Series res("", DType::BOOL, lhs.size());                                                   \
        auto &res_v = res._vector<bool>();                                                         \
        if constexpr (arithmetic_type<S>) {                                                        \
            switch (lhs.dtype()) {                                                                 \
                DataFrameCpp_Series_Scalar_Compare_Case_level1(BOOL, bool, op);                    \
                DataFrameCpp_Series_Scalar_Compare_Case_level1(INT, int, op);                      \
                DataFrameCpp_Series_Scalar_Compare_Case_level1(LONGLONG, long long, op);           \
                DataFrameCpp_Series_Scalar_Compare_Case_level1(FLOAT, float, op);                  \
                DataFrameCpp_Series_Scalar_Compare_Case_level1(DOUBLE, double, op);                \
            default:                                                                               \
                break;                                                                             \
            }                                                                                      \
        } else {                                                                                   \
            std::string info =                                                                     \
                std::format("Can't run bitwise operations on {}({}) and Scalar type {}.\n",        \
                            lhs.name(), lhs.dtype_name(), DType_traits<S>::name);                  \
            fmt::cerr(info);                                                                       \
            throw(std::runtime_error(info));                                                       \
        }                                                                                          \
        return res;                                                                                \
    }                                                                                              \
    template <supported_type S> Series operator op(const S &rhs, const Series &lhs) {              \
        return lhs op rhs;                                                                         \
    }
    DataFrameCpp_Series_Scalar_Bitwise_Bin_Op(&&);
    DataFrameCpp_Series_Scalar_Bitwise_Bin_Op(|| );

#undef DataFrameCpp_Series_Scalar_Compare
#undef DataFrameCpp_Series_Scalar_Compare_Case_level1
#undef DataFrameCpp_Series_Scalar_Compare_Case_level2
#undef DataFrameCpp_Series_Scalar_Compare_Transform

    // Pass a function object, or std::function
    template <bool inplace = false, supported_functor_type Func = std::function<double(double)>>
    auto Series::apply(Func f) {
        if constexpr (inplace) {
            using arg_type = typename FunctorType<Func>::arg_type;
            using ret_type = typename FunctorType<Func>::ret_type;
            if (DType_traits<arg_type>::dtype == dtype()) {
                if constexpr (std::is_same_v<arg_type, ret_type>) {
                    auto& data = _vector<arg_type>();
                    std::transform(data.begin(), data.end(), data.begin(), f);
                }
                else {
                    auto& data = _vector<arg_type>();
                    std::vector<ret_type> res_data(size());
                    std::transform(data.begin(), data.end(), res_data.begin(), f);
                    _values = res_data;
                }
            }
            return;
        }
        else {
            Series s(*this);
            s.apply<true, Func>(f);
            return s;
        }
    };

    // Note the consexpr if here.
#define DataFrameCpp_Series_Apply_Swith_Case_Stmt(type)                                            \
    if constexpr (boost::hana::contains(target_arg_types, boost::hana::type_c<type>)) {            \
        using functor_t = TemplateFunc<type>;                                                      \
        using ret_type = typename FunctorType<functor_t>::ret_type;                                \
        apply<true, functor_t>(functor_t{});                                                       \
    }                                                                                              \
    break
    template <bool inplace = false, template <typename> typename TemplateFunc,
        supported_type_pack... TargetArgTypes>
    auto Series::apply() {
        if constexpr (inplace) {
            auto target_arg_types = boost::hana::tuple_t<TargetArgTypes...>;
            switch (dtype()) {
            case DType::STRING:
                DataFrameCpp_Series_Apply_Swith_Case_Stmt(std::string);
            case DType::BOOL:
                DataFrameCpp_Series_Apply_Swith_Case_Stmt(bool);
            case DType::INT:
                DataFrameCpp_Series_Apply_Swith_Case_Stmt(int);
            case DType::LONGLONG:
                DataFrameCpp_Series_Apply_Swith_Case_Stmt(long long);
            case DType::FLOAT:
                DataFrameCpp_Series_Apply_Swith_Case_Stmt(float);
            case DType::DOUBLE:
                DataFrameCpp_Series_Apply_Swith_Case_Stmt(double);
            default:
                break;
            }
            return;
        }
        else {
            Series s(*this);
            s.apply<true, TemplateFunc>();
            return s;
        }
    };
#undef DataFrameCpp_Series_Apply_Swith_Case_Stmt

}; // namespace dfc
