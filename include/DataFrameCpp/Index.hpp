#pragma once
/**
 *@file
 *
 */
#include "Series.hpp"

namespace dfc {
    class ViewIndex;
    class SeriesView;

    class DataFrame;
    class DataFrameView;

    // Int64Positional index.
    // Example:
    //   Index pos: [2,0,1,3,4], gives the subscripts.
    class Index {
    public:
        Index() { set_info_(); }
        // explicit Index(std::shared_ptr<Series> &values) : _values(values) {}
        explicit Index(size_t sz) : _size(sz) { set_info_(); }
        explicit Index(const std::vector<size_t>& pos_) : _pos(pos_) {}
        Index(const std::shared_ptr<Index>& index, const std::vector<long long>& pos_)
            : _values(index->_values), _size(pos_.size()), _pos(index->iloc_id(pos_)) {
            set_info_();
        } // Can contains negative integer.
        Index(const std::shared_ptr<Index>& index, const std::vector<size_t>& pos_)
            : _values(index->_values), _size(pos_.size()), _pos(index->iloc_id_(pos_)) {
            set_info_();
        }
        // Index(std::shared_ptr<Index> &index) : {}

        size_t size() const { return _size; }
        void resize(size_t n) { _size = n; }

        virtual std::string name() const { return _values->name(); }
        IndexDType dtype() const { return _dtype; }
        std::string dtype_name() const { return _dtype_name; }

        // Subscripts.
        virtual size_t iloc_id(long long i) const { return _pos[i >= 0 ? i : _size - i]; }
        virtual std::vector<size_t> iloc_id(const std::vector<long long>& pos);

        virtual size_t iloc_id_(size_t i) const { return _pos[i]; } // Only for positive index.
        virtual std::vector<size_t> iloc_id_(const std::vector<size_t>& pos);

        virtual std::string iloc_str(long long i) const { return _values->iloc_str(i); }

        // Key-position map.
        virtual std::vector<size_t> loc(const long long& key) const {
            throw std::logic_error{ "Not implemented." };
        }
        virtual std::vector<size_t> loc(const std::vector<long long>& keys) const {
            throw std::logic_error{ "Not implemented." };
        }
        virtual std::vector<size_t> loc(const std::string& key) const {
            throw std::logic_error{ "Not implemented." };
        }
        virtual std::vector<size_t> loc(const std::vector<std::string>& keys) const {
            throw std::logic_error{ "Not implemented." };
        }
        virtual void push_back() { ++_size; }

        virtual std::shared_ptr<Index> clone() const;

    protected:
        friend DataFrameView;
        friend DataFrame;

        std::shared_ptr<Series> _values;
        size_t _size = 0;

        std::vector<size_t> _pos; ///< map indices in current dataframe to indices in _values.

        IndexDType _dtype = IndexDType::Positional;
        std::string _dtype_name = "Positional";

        virtual void set_info_() {}
    };

    class Int64RangeIndex : public Index {
    public:
        Int64RangeIndex() {}
        explicit Int64RangeIndex(size_t sz) : Index(sz) { _end = _start + sz; }

        size_t iloc_id(long long i) const override { return i >= 0 ? i : _end + i; }
        size_t iloc_id_(size_t i) const override { return _start + i; } // Only for positive index.
        // std::string iloc_str(long long i) const override { return _values.iloc_str(i); }

        std::vector<size_t> loc(long long key) const { return { static_cast<size_t>(key - _start) }; }

    private:
        long long _start = 0; // start position in data.
        long long _end = 0;   // not included.

        void set_info_() override {
            _dtype = IndexDType::Int64Range;
            _dtype_name = "Int64Range";
        }
    };

    class Int64PeriodIndex : public Index {
    public:
        size_t iloc_id_(size_t i) const override {
            return _start + i * _diff;
        } // Only for positive index.
        size_t iloc_id(long long i) const override { return i > 0 ? iloc_id_(i) : _end + i * _diff; }
        std::string iloc_str(long long i) const override { return _values->iloc_str(i); }

        // size_t loc(long long key) const override { return key - _start; }

    private:
        long long _start = 0, _diff = 0, _end = 0; // end_is the value of last index +_diff
    };

    template <typename KeyType> class KeyValueIndex : public Index {
    public:
        std::vector<size_t> loc(const KeyType& key) const override {
            if (auto it = _kv.find(key); it != _kv.end()) {
                return it->second;
            }
            else {
                return {};
            }
        }
        std::vector<size_t> loc(const std::vector<KeyType>& keys) const override {
            std::vector<size_t> res;
            for (const auto& key : keys) {
                if (auto it = _kv.find(key); it != _kv.end()) {
                    res.insert(res.begin(), it->second.begin(), it->second.end());
                }
            }
            return res;
        }

    protected:
        std::unordered_map<KeyType, std::vector<size_t>> _kv;

        void set_info_() override;
    };

    using Int64Index = KeyValueIndex<long long>;

    template <> inline void KeyValueIndex<long long>::set_info_() {
        _dtype = IndexDType::Int64;
        _dtype_name = "Int64";
    }

    /**
     * \brief Convert a series to a index.
     * \param a
     * \return a
     */
    inline Index create_index(Series* values) {
        return Index();
    }

}; // namespace dfc
