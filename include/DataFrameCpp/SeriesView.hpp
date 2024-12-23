#pragma once

// @kilasuelika
// May 19, 2022

#include "Index.hpp"
#include <span>

namespace dfc {

    template <typename T> struct SeriesViewIterator;

    class DataFrame;

    // class Series;

    class SeriesView {
    public:
        SeriesView() {}

        explicit SeriesView(Series* series);
        SeriesView(Series* series, const std::vector<size_t>& pos);
        SeriesView(Series* series, const std::vector<long long>& pos);
        SeriesView(const SeriesView* view, const std::vector<size_t>& pos);
        // SeriesView(Series *series, const Index &index);
        SeriesView(Series* series, std::shared_ptr<Index>& index);

        friend Series;

        // info
        std::string name() const;
        size_t size() const; // size of _index
        DType dtype() const;
        std::string dtype_name() const;
        bool empty() const;
        // ViewIndexType index_type() const;

        // Subscript
        template <typename T> const T& iloc_(size_t i) const;
        template <typename T> T& iloc_(size_t i);

        std::string iloc_str_(long long i) const;
        std::string iloc_str(long long i) const;
        // std::ostream &operator<<(std::ostream &os, const SeriesView &df);
        // template <typename T> SeriesViewIterator<T> begin() const;
        // template <typename T> SeriesViewIterator<T> end() const;

        // Modification
        template<typename T> SeriesView& set(const T& v);
        template <typename T> void astype();

        Series copy() const;

        // Assignment
        SeriesView& operator=(const SeriesView& obj);
        template <typename T> SeriesView& operator=(const T& v);

        template <typename T1, typename T2> SeriesView& assign(const SeriesView& obj);

        SeriesView& operator+=(const SeriesView& obj);
        SeriesView& operator-=(const SeriesView& obj);
        SeriesView& operator*=(const SeriesView& obj);
        SeriesView& operator/=(const SeriesView& obj);

        SeriesView& operator+=(const Series& obj);
        SeriesView& operator-=(const Series& obj);
        SeriesView& operator*=(const Series& obj);
        SeriesView& operator/=(const Series& obj);

        // Operator.
        template <typename T1, typename T2, typename Ret> Series add(const SeriesView& obj);
        template <typename T1, typename T2, typename Ret> Series sub(const SeriesView& obj);
        template <typename T1, typename T2, typename Ret> Series mul(const SeriesView& obj);
        template <typename T1, typename T2, typename Ret> Series div(const SeriesView& obj);

        template <typename T1, typename T2, typename Ret> Series add(const Series& obj);
        template <typename T1, typename T2, typename Ret> Series sub(const Series& obj);
        template <typename T1, typename T2, typename Ret> Series mul(const Series& obj);
        template <typename T1, typename T2, typename Ret> Series div(const Series& obj);

        template <typename T1, typename T2> void add_assignment(const Series& obj);
        template <typename T1, typename T2> void sub_assignment(const Series& obj);
        template <typename T1, typename T2> void mul_assignment(const Series& obj);
        template <typename T1, typename T2> void div_assignment(const Series& obj);

        template <typename T1, typename T2> void add_assignment(const SeriesView& obj);
        template <typename T1, typename T2> void sub_assignment(const SeriesView& obj);
        template <typename T1, typename T2> void mul_assignment(const SeriesView& obj);
        template <typename T1, typename T2> void div_assignment(const SeriesView& obj);

        Series operator+(const SeriesView& obj);
        Series operator-(const SeriesView& obj);
        Series operator*(const SeriesView& obj);
        Series operator/(const SeriesView& obj);

        Series operator+(const Series& obj);
        Series operator-(const Series& obj);
        Series operator*(const Series& obj);
        Series operator/(const Series& obj);

        friend Series operator>(const SeriesView& lhs, const SeriesView& rhs);
        friend Series operator<(const SeriesView& lhs, const SeriesView& rhs);
        friend Series operator>=(const SeriesView& lhs, const SeriesView& rhs);
        friend Series operator<=(const SeriesView& lhs, const SeriesView& rhs);
        friend Series operator==(const SeriesView& lhs, const SeriesView& rhs);
        friend Series operator!=(const SeriesView& lhs, const SeriesView& rhs);
        friend Series operator&&(const SeriesView& lhs, const SeriesView& rhs);
        friend Series operator||(const SeriesView& lhs, const SeriesView& rhs);

        template <supported_type S> friend Series operator>(const SeriesView& lhs, const S& rhs);
        template <supported_type S> friend Series operator<(const SeriesView& lhs, const S& rhs);
        template <supported_type S> friend Series operator>=(const SeriesView& lhs, const S& rhs);
        template <supported_type S> friend Series operator<=(const SeriesView& lhs, const S& rhs);
        template <supported_type S> friend Series operator==(const SeriesView& lhs, const S& rhs);
        template <supported_type S> friend Series operator!=(const SeriesView& lhs, const S& rhs);
        template <supported_type S> friend Series operator&&(const SeriesView& lhs, const S& rhs);
        template <supported_type S> friend Series operator||(const SeriesView& lhs, const S& rhs);

        template <supported_type S> friend Series operator>(const S& rhs, const SeriesView& lhs);
        template <supported_type S> friend Series operator<(const S& rhs, const SeriesView& lhs);
        template <supported_type S> friend Series operator>=(const S& rhs, const SeriesView& lhs);
        template <supported_type S> friend Series operator<=(const S& rhs, const SeriesView& lhs);
        template <supported_type S> friend Series operator==(const S& rhs, const SeriesView& lhs);
        template <supported_type S> friend Series operator!=(const S& rhs, const SeriesView& lhs);
        template <supported_type S> friend Series operator&&(const S& rhs, const SeriesView& lhs);
        template <supported_type S> friend Series operator||(const S& rhs, const SeriesView& lhs);

        // Data
        template <typename T> const std::vector<T>& vector() const;

        friend std::ostream& operator<<(std::ostream& os, const SeriesView& dv);
        template <typename T> friend class SeriesViewIterator;

    private:
        // std::shared_ptr<ViewIndex> _index;
        std::shared_ptr<Index> _index;
        Series* _values;
        // DataFrame *_data;
        // size_t _id;
        template <typename T> std::vector<T>& _vector();
    };

    template <typename T> struct SeriesViewIterator {
        SeriesViewIterator(SeriesView* data);

        SeriesViewIterator& operator++();

        T operator*();

        SeriesView* data;
        T* current;
        size_t i;
    };
}; // namespace dfc
