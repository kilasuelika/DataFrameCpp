#define DataFrameCpp_ASSGNMENT_SWITCH_CASE_(FUNC, PRE)                                             \
    switch (dtype()) {                                                                             \
    case DType::BOOL:                                                                              \
        switch (obj.dtype()) {                                                                     \
        case DType::BOOL:                                                                          \
            PRE FUNC<bool, bool>(obj);                                                             \
            break;                                                                                 \
        case DType::INT:                                                                           \
            PRE FUNC<int, int>(obj);                                                               \
            break;                                                                                 \
        case DType::LONGLONG:                                                                      \
            PRE FUNC<long long, long long>(obj);                                                   \
            break;                                                                                 \
        case DType::FLOAT:                                                                         \
            PRE FUNC<float, float>(obj);                                                           \
            break;                                                                                 \
        case DType::DOUBLE:                                                                        \
            PRE FUNC<double, double>(obj);                                                         \
            break;                                                                                 \
        default:                                                                                   \
            break;                                                                                 \
        }                                                                                          \
        break;                                                                                     \
    case DType::INT:                                                                               \
        switch (obj.dtype()) {                                                                     \
        case DType::BOOL:                                                                          \
            PRE FUNC<int, bool>(obj);                                                              \
            break;                                                                                 \
        case DType::INT:                                                                           \
            PRE FUNC<int, int>(obj);                                                               \
            break;                                                                                 \
        case DType::LONGLONG:                                                                      \
            PRE FUNC<long long, long long>(obj);                                                   \
            break;                                                                                 \
        case DType::FLOAT:                                                                         \
            PRE FUNC<float, float>(obj);                                                           \
            break;                                                                                 \
        case DType::DOUBLE:                                                                        \
            PRE FUNC<double, double>(obj);                                                         \
            break;                                                                                 \
        default:                                                                                   \
            break;                                                                                 \
        }                                                                                          \
        break;                                                                                     \
    case DType::LONGLONG:                                                                          \
        switch (obj.dtype()) {                                                                     \
        case DType::BOOL:                                                                          \
            PRE FUNC<long long, bool>(obj);                                                        \
            break;                                                                                 \
        case DType::INT:                                                                           \
            PRE FUNC<long long, int>(obj);                                                         \
            break;                                                                                 \
        case DType::LONGLONG:                                                                      \
            PRE FUNC<long long, long long>(obj);                                                   \
            break;                                                                                 \
        case DType::FLOAT:                                                                         \
            PRE FUNC<double, float>(obj);                                                          \
            break;                                                                                 \
        case DType::DOUBLE:                                                                        \
            PRE FUNC<double, double>(obj);                                                         \
            break;                                                                                 \
        default:                                                                                   \
            break;                                                                                 \
        }                                                                                          \
        break;                                                                                     \
    case DType::FLOAT:                                                                             \
        switch (obj.dtype()) {                                                                     \
        case DType::BOOL:                                                                          \
            PRE FUNC<float, bool>(obj);                                                            \
            break;                                                                                 \
        case DType::INT:                                                                           \
            PRE FUNC<float, int>(obj);                                                             \
            break;                                                                                 \
        case DType::LONGLONG:                                                                      \
            PRE FUNC<double, long long>(obj);                                                      \
            break;                                                                                 \
        case DType::FLOAT:                                                                         \
            PRE FUNC<float, float>(obj);                                                           \
            break;                                                                                 \
        case DType::DOUBLE:                                                                        \
            PRE FUNC<double, double>(obj);                                                         \
            break;                                                                                 \
        default:                                                                                   \
            break;                                                                                 \
        }                                                                                          \
        break;                                                                                     \
    case DType::DOUBLE:                                                                            \
        switch (obj.dtype()) {                                                                     \
        case DType::BOOL:                                                                          \
            PRE FUNC<double, bool>(obj);                                                           \
            break;                                                                                 \
        case DType::INT:                                                                           \
            PRE FUNC<double, int>(obj);                                                            \
            break;                                                                                 \
        case DType::LONGLONG:                                                                      \
            PRE FUNC<double, long long>(obj);                                                      \
            break;                                                                                 \
        case DType::FLOAT:                                                                         \
            PRE FUNC<double, float>(obj);                                                          \
            break;                                                                                 \
        case DType::DOUBLE:                                                                        \
            PRE FUNC<double, double>(obj);                                                         \
            break;                                                                                 \
        default:                                                                                   \
            break;                                                                                 \
        }                                                                                          \
        break;                                                                                     \
    default:                                                                                       \
        break;                                                                                     \
    }

//#define
#define DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(LHS_TYPE, RHS_TYPE, FUNC, OP)                      \
    template <typename T1, typename T2, typename Ret> Series LHS_TYPE::FUNC(const RHS_TYPE &obj) { \
        size_t N = size();                                                                         \
        std::vector<Ret> res(N);                                                                   \
        for (size_t i = 0; i < N; ++i) {                                                           \
            res[i] = iloc_<T1>(i) OP obj.iloc_<T2>(i);                                             \
        }                                                                                          \
        return Series(std::move(res));                                                                     \
    }

#define DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(LHS_TYPE, RHS_TYPE, FUNC, OP)               \
  inline Series LHS_TYPE::operator OP(const RHS_TYPE &obj) {                                            \
        Series res;                                                                                \
        switch (dtype()) {                                                                         \
        case DType::BOOL:                                                                          \
            switch (obj.dtype()) {                                                                 \
            case DType::BOOL:                                                                      \
                res = FUNC<bool, bool, bool>(obj);                                                 \
                break;                                                                             \
            case DType::INT:                                                                       \
                res = FUNC<int, int, int>(obj);                                                    \
                break;                                                                             \
            case DType::LONGLONG:                                                                  \
                res = FUNC<long long, long long, long long>(obj);                                  \
                break;                                                                             \
            case DType::FLOAT:                                                                     \
                res = FUNC<float, float, float>(obj);                                              \
                break;                                                                             \
            case DType::DOUBLE:                                                                    \
                res = FUNC<double, double, double>(obj);                                           \
                break;                                                                             \
            default:                                                                               \
                break;                                                                             \
            }                                                                                      \
            break;                                                                                 \
        case DType::INT:                                                                           \
            switch (obj.dtype()) {                                                                 \
            case DType::BOOL:                                                                      \
                res = FUNC<int, bool, int>(obj);                                                   \
                break;                                                                             \
            case DType::INT:                                                                       \
                res = FUNC<int, int, int>(obj);                                                    \
                break;                                                                             \
            case DType::LONGLONG:                                                                  \
                res = FUNC<long long, long long, long long>(obj);                                  \
                break;                                                                             \
            case DType::FLOAT:                                                                     \
                res = FUNC<float, float, float>(obj);                                              \
                break;                                                                             \
            case DType::DOUBLE:                                                                    \
                res = FUNC<double, double, double>(obj);                                           \
                break;                                                                             \
            default:                                                                               \
                break;                                                                             \
            }                                                                                      \
            break;                                                                                 \
        case DType::LONGLONG:                                                                      \
            switch (obj.dtype()) {                                                                 \
            case DType::BOOL:                                                                      \
                res = FUNC<long long, bool, long long>(obj);                                       \
                break;                                                                             \
            case DType::INT:                                                                       \
                res = FUNC<long long, int, long long>(obj);                                        \
                break;                                                                             \
            case DType::LONGLONG:                                                                  \
                res = FUNC<long long, long long, long long>(obj);                                  \
                break;                                                                             \
            case DType::FLOAT:                                                                     \
                res = FUNC<long long, float, double>(obj);                                         \
                break;                                                                             \
            case DType::DOUBLE:                                                                    \
                res = FUNC<long long, double, double>(obj);                                        \
                break;                                                                             \
            default:                                                                               \
                break;                                                                             \
            }                                                                                      \
            break;                                                                                 \
        case DType::FLOAT:                                                                         \
            switch (obj.dtype()) {                                                                 \
            case DType::BOOL:                                                                      \
                res = FUNC<float, bool, float>(obj);                                               \
                break;                                                                             \
            case DType::INT:                                                                       \
                res = FUNC<float, int, float>(obj);                                                \
                break;                                                                             \
            case DType::LONGLONG:                                                                  \
                res = FUNC<float, long long, double>(obj);                                         \
                break;                                                                             \
            case DType::FLOAT:                                                                     \
                res = FUNC<float, float, float>(obj);                                              \
                break;                                                                             \
            case DType::DOUBLE:                                                                    \
                res = FUNC<float, double, double>(obj);                                            \
                break;                                                                             \
            default:                                                                               \
                break;                                                                             \
            }                                                                                      \
            break;                                                                                 \
        case DType::DOUBLE:                                                                        \
            switch (obj.dtype()) {                                                                 \
            case DType::BOOL:                                                                      \
                res = FUNC<double, bool, double>(obj);                                             \
                break;                                                                             \
            case DType::INT:                                                                       \
                res = FUNC<double, int, double>(obj);                                              \
                break;                                                                             \
            case DType::LONGLONG:                                                                  \
                res = FUNC<double, long long, double>(obj);                                        \
                break;                                                                             \
            case DType::FLOAT:                                                                     \
                res = FUNC<double, float, double>(obj);                                            \
                break;                                                                             \
            case DType::DOUBLE:                                                                    \
                res = FUNC<double, double, double>(obj);                                           \
                break;                                                                             \
            default:                                                                               \
                break;                                                                             \
            }                                                                                      \
            break;                                                                                 \
        default:                                                                                   \
            break;                                                                                 \
        }                                                                                          \
        return res;                                                                                \
    }

#define DataFrameCpp_SERIES_SERIES_ARITH_BIN_OP_IMPL_(OP, FUNCNAME)                                \
  inline Series Series::operator OP(const Series &obj) {                                                \
        Series res(*this);                                                                    \
        DataFrameCpp_ASSGNMENT_SWITCH_CASE_(FUNCNAME, res.);                                       \
        return res;                                                                                \
    }

#define DataFrameCpp_DATAFRAME_OR_DATAFRAMEVIEW_BIN_OP_IMPL_(LHS_TYPE, RHS_TYPE, OP,               \
    SERIES_OP_ASSIGNMENT_FUNCNAME)        \
  inline DataFrame LHS_TYPE::operator OP(const RHS_TYPE &obj) {                                         \
        DataFrame res(*this);                                                                      \
        if (_shape != obj._shape) {                                                                \
            std::cout                                                                              \
                << std::format(                                                                    \
                       "[DataFrame]: two dataframes have different shapes: ({}, {}), ({}, {}).",   \
                       _shape[0], _shape[1], obj._shape[0], obj._shape[1])                         \
                << std::endl;                                                                      \
        } else {                                                                                   \
            for (size_t c = 0; c < _shape[1]; ++i) {                                               \
                res._values[c].SERIES_OP_ASSIGNMENT_FUNCNAME(obj._values[c]);                      \
            }                                                                                      \
        }                                                                                          \
        return res;                                                                                \
    }

#define DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(LHS_TYPE, RHS_TYPE, OP, OPNAME)         \
    template <typename T1, typename T2> void LHS_TYPE::OPNAME##_assignment(const RHS_TYPE &obj) {  \
        if (DTypeMap[std::type_index(typeid(T1))] != dtype()) {                                    \
            astype<T1>();                                                                          \
        }                                                                                          \
        const auto sz = size();                                                                    \
        for (size_t i = 0; i < sz; ++i) {                                                          \
            iloc_<T1>(i) OP obj.iloc_<T2>(i);                                                      \
        }                                                                                          \
    }

#define DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(LHS_TYPE, RHS_TYPE, op,            \
    MEMBER_NAME)                       \
   inline LHS_TYPE &LHS_TYPE::operator op(const RHS_TYPE &obj) {                                         \
        if (size() != obj.size()) {                                                                \
            std::string info = std::format(                                                        \
                "[" #LHS_TYPE "]: two series has different length: {}, {} for " #op ".", size(),   \
                obj.size());                                                                       \
            std::cerr << info << std::endl;                                                        \
            return *this;                                                                          \
        } else {                                                                                   \
            if ((static_cast<int>(dtype()) <= 1) || (static_cast<int>(obj.dtype()) <= 1)) {        \
                std::string info = std::format(                                                    \
                    "[" #LHS_TYPE                                                                  \
                    "]: Arithmetic operations are not allowed on non-numeric series for " #op      \
                    ". ");                                                                         \
                std::cerr << info << std::endl;                                                    \
                return *this;                                                                      \
            } else {                                                                               \
                DataFrameCpp_ASSGNMENT_SWITCH_CASE_(MEMBER_NAME, );                                \
                return *this;                                                                      \
            }                                                                                      \
        }                                                                                          \
    }
