#define DataFrameCpp_DataFrame_Or_View_Scalar_Bin_Op(type, op)                                     \
    template <supported_type S> inline DataFrame operator op(const type &lhs, const S &rhs) {      \
        DataFrame res(lhs._index);                                                                 \
        fmt::print("New df: shape {} {}\n", res.shape()[0], res.shape()[1]);                       \
        for (int i = 0; i < lhs._shape[1]; ++i) {                                                  \
            fmt::print("{}\n", i);                                                                 \
            res.append_col(std::move(*(lhs._values[i])op rhs));                                    \
        }                                                                                          \
        fmt::print("New df: shape {} {}\n", res.shape()[0], res.shape()[1]);                       \
        return res;                                                                                \
    }                                                                                              \
    template <supported_type S> inline DataFrame operator op(const S &rhs, const type &lhs) {      \
        return lhs op rhs;                                                                         \
    }
