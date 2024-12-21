#pragma once

// @kilasuelika
// May 19, 2022

#include "SeriesView.hpp"
#include <eigen3/Eigen/Dense>
// #include <optional>
// #include <unordered_map>
#include <fstream>
#include <map>
#include "Index.hpp"

namespace dfc {

    class DataFrameView;

    class DataFrame {
    public:
        friend DataFrameView;

        // Constructor
        DataFrame() {}
        explicit DataFrame(std::initializer_list<Series> columns);
        // Blank DataFrame.
        DataFrame(const std::vector<std::string>& columns, const std::vector<DType>& types);

        DataFrame(const DataFrame& df);
        explicit DataFrame(const DataFrameView& view);
        explicit DataFrame(std::shared_ptr<Index> index);
        // explicit DataFrame(std::shared_ptr<ViewIndex> index);

        // info
        std::vector<DType> dtypes() const;
        std::string dtype_name(int i) const { return _values[i]->dtype_name(); }
        const DataFrameShape& shape() const;
        bool empty() const;
        DataFrameView head(int n = 5) const;
        void describe() const; // print column name and type.

        std::vector<std::string> columns() const;
        /// Check if there are columns with specified name.
        bool contains(const std::string& colname) const { return _column_map.contains(colname); }
        /**
         * \brief Check if a column is unique in dataframe.
         * \param a
         * \return a
         */
        bool contains_unique(const std::string& colname) const;
        /**
         * \brief Rename columns.
         * \param map Old name to new name map.
         */
        void rename(const std::map<std::string, std::string>& map);

        // Subscripts.
        // SeriesView operator[](const std::string &col);
        DataFrameView operator[](const std::string& col) const;
        DataFrameView operator[](const DataFrame& col) const;

        template <typename T> T& iloc(long long i, long long j = 0);
        // SeriesView iloc(const std::vector<long long> &rows, const std::string &col);
        DataFrameView iloc(const std::vector<long long>& rows, const std::vector<long long>& cols);
        DataFrameView iloc(const std::vector<long long>& rows, const std::vector<std::string>& cols);
        DataFrameView iloc(const std::vector<long long>& rows,
            std::initializer_list<const char*> cols);
        // DataFrameView iloc(const std::vector<long long> &rows, const std::vector<const char *>
        // &cols);

        //// Set column value
        //template<typename T>
        //DataFrameView set(const std::string& colname, const T& v);

        std::string iloc_str_(size_t i, size_t j); // Always positive integer.

        // Modify
        void insert_col(int k, const std::string& name, DType type);
        void insert_col(int k, Series&& column);
        void insert_col(int k, const std::string& name, Series column);

        void append_col(const std::string& name, DType type); // blank column
        void append_col(Series&& column);
        void append_col(const std::string& name, Series column);
        void append_cols(std::vector<Series>&& columns);

        /**
         * \brief Remove a column.
         * \param a
         */
        template <bool inplace = false> decltype(auto) remove_col(const std::string& name);
        template <bool inplace = false> decltype(auto) remove_col(const std::vector<std::string>& names);

        /**
         * \brief Drop a column by integer index.
         * \param col_id Column index to be removed, can be negative.
         */
        template <bool inplace = false> decltype(auto) remove_col(int col_id);
        /**
         * \brief Drop multiple columns.
         * \param a
         */
        template <bool inplace = false> decltype(auto) remove_cols(const std::vector<int>& col_ids);

        // Insert a new blank row.
        DataFrameView append_row();

        // Index
        template <bool inplace = false> decltype(auto) set_index(const std::string& name);

        // Arithmetic
        friend inline DataFrame operator+(const DataFrame& lhs, const DataFrame& rhs);
        friend inline DataFrame operator-(const DataFrame& lhs, const DataFrame& rhs);
        friend inline DataFrame operator*(const DataFrame& lhs, const DataFrame& rhs);
        friend inline DataFrame operator/(const DataFrame& lhs, const DataFrame& rhs);

        friend inline DataFrame operator+(const DataFrame& lhs, const DataFrameView& rhs);
        friend inline DataFrame operator-(const DataFrame& lhs, const DataFrameView& rhs);
        friend inline DataFrame operator*(const DataFrame& lhs, const DataFrameView& rhs);
        friend inline DataFrame operator/(const DataFrame& lhs, const DataFrameView& rhs);

        friend inline DataFrame operator+(const DataFrameView& lhs, const DataFrame& rhs);
        friend inline DataFrame operator-(const DataFrameView& lhs, const DataFrame& rhs);
        friend inline DataFrame operator*(const DataFrameView& lhs, const DataFrame& rhs);
        friend inline DataFrame operator/(const DataFrameView& lhs, const DataFrame& rhs);

        DataFrame& operator+=(const DataFrameView& rhs);
        DataFrame& operator-=(const DataFrameView& rhs);
        DataFrame& operator*=(const DataFrameView& rhs);
        DataFrame& operator/=(const DataFrameView& rhs);

        // Scalar boolean
        template <supported_type S> friend DataFrame operator>(const DataFrame& lhs, const S& rhs);
        template <supported_type S> friend DataFrame operator<(const DataFrame& lhs, const S& rhs);
        template <supported_type S> friend DataFrame operator>=(const DataFrame& lhs, const S& rhs);
        template <supported_type S> friend DataFrame operator<=(const DataFrame& lhs, const S& rhs);
        template <supported_type S> friend DataFrame operator==(const DataFrame& lhs, const S& rhs);
        template <supported_type S> friend DataFrame operator!=(const DataFrame& lhs, const S& rhs);
        template <supported_type S> friend DataFrame operator&&(const DataFrame& lhs, const S& rhs);
        template <supported_type S> friend DataFrame operator||(const DataFrame& lhs, const S& rhs);

        template <supported_type S> friend DataFrame operator>(const S& rhs, const DataFrame& lhs);
        template <supported_type S> friend DataFrame operator<(const S& rhs, const DataFrame& lhs);
        template <supported_type S> friend DataFrame operator>=(const S& rhs, const DataFrame& lhs);
        template <supported_type S> friend DataFrame operator<=(const S& rhs, const DataFrame& lhs);
        template <supported_type S> friend DataFrame operator==(const S& rhs, const DataFrame& lhs);
        template <supported_type S> friend DataFrame operator!=(const S& rhs, const DataFrame& lhs);
        template <supported_type S> friend DataFrame operator&&(const S& rhs, const DataFrame& lhs);
        template <supported_type S> friend DataFrame operator||(const S& rhs, const DataFrame& lhs);

        // conversion
        DataFrame copy() const;

        bool all_numeric_dtype() const;

        // All numeric columns to an eigen type.
        template <typename T> T to_eigen() const;
        // Specific columns.
        template <typename T> T to_eigen(const std::vector<std::string>& l) const;

        // Data
        const std::vector<Series*>& values() const;
        // const std::unordered_map<std::string, size_t> &get_column_map() const;
        //  IO
        friend std::ostream& operator<<(std::ostream& os, const DataFrame& dt);
        void to_csv(const std::string& filename, const CSVIOOptions& options = CSVIOOptions()) const;

        template <bool inplace = false, supported_functor_type Func = std::function<double(double)>>
        decltype(auto) apply(Func f);
        template <supported_functor_type Func = std::function<double(double)>>
        decltype(auto) apply_in_place(Func f);

        template <bool inplace, template <typename> typename TemplateFunc,
            supported_type_pack... TargetArgumentTypes>
        decltype(auto) apply();

        // Implemented in math.hpp
#define DataFrameCpp_DataFrame_Unary_Math_Function_Declaration(name)                               \
    template <bool inplace = false> auto name()

        DataFrameCpp_DataFrame_Unary_Math_Function_Declaration(sin);
        DataFrameCpp_DataFrame_Unary_Math_Function_Declaration(cos);
        DataFrameCpp_DataFrame_Unary_Math_Function_Declaration(tan);

#undef DataFrameCpp_Unary_Math_Function_Declaration

        ~DataFrame();

        std::vector<Series*> _values;
        // std::shared_ptr<Index> _index = std::make_shared<Index>();
        std::shared_ptr<Index> _index = std::make_shared<Int64RangeIndex>();

    private:
        DataFrameShape _shape{ 0, 0 };
        // std::unordered_map<std::string, std::vector<size_t>> _column_map;
        std::multimap<std::string, size_t> _column_map;
        std::string _find_valid_column_name(const std::string& name);
        // void _init_by_columns(std::vector<Series> &&columns);
    };

}; // namespace dfc
