#pragma once

// @kilasuelika
// Sep 24, 2022
#include<cmath>
#include<functional>


namespace dfc {

#define DataFrameCpp_STD_FUNC_TO_My_FUNC(name)                                                     \
    template <typename T> inline static std::function<T(T)> name = [](T x) { return std::name(x); };

DataFrameCpp_STD_FUNC_TO_My_FUNC(sin);
DataFrameCpp_STD_FUNC_TO_My_FUNC(cos);
DataFrameCpp_STD_FUNC_TO_My_FUNC(tan);

#undef DataFrameCpp_STD_FUNC_TO_My_FUNC

} // namespace dfc
