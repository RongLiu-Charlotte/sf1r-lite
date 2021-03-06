#ifndef SF1R_COMMON_NUMERIC_PROPERTY_TABLE_H
#define SF1R_COMMON_NUMERIC_PROPERTY_TABLE_H

#include "NumericPropertyTableBase.h"
#include <util/modp_numtoa.h>

#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace sf1r
{

template <class T>
struct InvalidGreat
{
    T invalidValue_;

    InvalidGreat(const T& invalid) : invalidValue_(invalid) {}

    bool operator()(const T& x, const T& y) const
    {
        if (x == invalidValue_)
            return false;

        if (y == invalidValue_)
            return true;

        return x < y;
    }
};

template <class T>
struct InvalidLess
{
    T invalidValue_;

    InvalidLess(const T& invalid) : invalidValue_(invalid) {}

    bool operator()(const T& x, const T& y) const
    {
        if (y == invalidValue_)
            return false;

        if (x == invalidValue_)
            return true;

        return x < y;
    }
};

template <class T>
class NumericPropertyTable : public NumericPropertyTableBase
{
public:
	//define sf1r search precision
	//float normally used as price
	//double normally used as latitude/longitude
	enum
	{
		kPrecisionFloat = 2,
		kPrecisionDouble = 6
	};

public:
    NumericPropertyTable(PropertyDataType type)
        : NumericPropertyTableBase(type)
        , dirty_(false)
        , invalidValue_(std::numeric_limits<T>::max())
        , data_(1, invalidValue_)
    {
    }

    NumericPropertyTable(PropertyDataType type, const T& initValue)
        : NumericPropertyTableBase(type)
        , dirty_(false)
        , invalidValue_(initValue)
        , data_(1, invalidValue_)
    {
    }

    ~NumericPropertyTable()
    {
    }

    void init(const std::string& path)
    {
        path_ = path;
        std::ifstream ifs(path_.c_str());
        if (ifs) load_(ifs);
    }

    void resize(std::size_t size)
    {
        ScopedWriteLock lock(mutex_);
        data_.resize(size, invalidValue_);
    }

    std::size_t size(bool isLock = true) const
    {
        ScopedReadBoolLock lock(mutex_, isLock);
        return data_.size();
    }

    void flush()
    {
        if (!dirty_) return;
        dirty_ = false;
        std::vector<T>(data_).swap(data_);
        if (path_.empty()) return;
        std::ofstream ofs(path_.c_str());
        if (ofs) save_(ofs);
    }

    bool isValid(std::size_t pos, bool isLock) const
    {
        ScopedReadBoolLock lock(mutex_, isLock);
        if (pos >= data_.size() || data_[pos] == invalidValue_)
            return false;
        return true;
    }

    bool getInt32Value(std::size_t pos, int32_t& value, bool isLock) const
    {
        ScopedReadBoolLock lock(mutex_, isLock);
        if (pos >= data_.size() || data_[pos] == invalidValue_)
            return false;

        value = static_cast<int32_t>(data_[pos]);
        return true;
    }
    bool getFloatValue(std::size_t pos, float& value, bool isLock) const
    {
        ScopedReadBoolLock lock(mutex_, isLock);
        if (pos >= data_.size() || data_[pos] == invalidValue_)
            return false;

        value = static_cast<float>(data_[pos]);
        return true;
    }
    bool getInt64Value(std::size_t pos, int64_t& value, bool isLock) const
    {
        ScopedReadBoolLock lock(mutex_, isLock);
        if (pos >= data_.size() || data_[pos] == invalidValue_)
            return false;

        value = static_cast<int64_t>(data_[pos]);
        return true;
    }
    bool getDoubleValue(std::size_t pos, double& value, bool isLock) const
    {
        ScopedReadBoolLock lock(mutex_, isLock);
        if (pos >= data_.size() || data_[pos] == invalidValue_)
            return false;

        value = static_cast<double>(data_[pos]);
        return true;
    }
    bool getStringValue(std::size_t pos, std::string& value, bool isLock) const
    {
        ScopedReadBoolLock lock(mutex_, isLock);
        if (pos >= data_.size() || data_[pos] == invalidValue_)
            return false;

        value = boost::lexical_cast<std::string>(data_[pos]);
        return true;
    }
    bool getDoublePairValue(std::size_t pos, std::pair<double, double>& value, bool isLock) const
    {
        ScopedReadBoolLock lock(mutex_, isLock);
        if (pos >= data_.size() || data_[pos] == invalidValue_)
            return false;

        value.first = value.second = static_cast<double>(data_[pos]);
        return true;
    }
    bool getInt64PairValue(std::size_t pos, std::pair<int64_t, int64_t>& value, bool isLock) const
    {
        ScopedReadBoolLock lock(mutex_, isLock);
        if (pos >= data_.size() || data_[pos] == invalidValue_)
            return false;

        value.first = value.second = static_cast<int64_t>(data_[pos]);
        return true;
    }


    bool getFloatMinValue(float& minValue, bool isLock) const
    {
        ScopedReadBoolLock lock(mutex_, isLock);

        typename std::vector<T>::const_iterator minIter = std::min_element(
            data_.begin(), data_.end(), InvalidGreat<T>(invalidValue_));

        if (minIter == data_.end() || *minIter == invalidValue_)
            return false;

        minValue = static_cast<float>(*minIter);
        return true;
    }

    bool getFloatMaxValue(float& maxValue, bool isLock) const
    {
        ScopedReadBoolLock lock(mutex_, isLock);

        typename std::vector<T>::const_iterator maxIter = std::max_element(
            data_.begin(), data_.end(), InvalidLess<T>(invalidValue_));

        if (maxIter == data_.end() || *maxIter == invalidValue_)
            return false;

        maxValue = static_cast<float>(*maxIter);
        return true;
    }

    bool getValue(std::size_t pos, T& value, bool isLock = true) const
    {
        ScopedReadBoolLock lock(mutex_, isLock);
        if (pos >= data_.size() || data_[pos] == invalidValue_)
            return false;

        value = data_[pos];
        return true;
    }

    void* getValueList()
    {
        if (!data_.empty())
            return static_cast<void*>(&data_[0]);
        else
            return NULL;
    }
    const void* getValueList() const
    {
        if (!data_.empty())
            return static_cast<const void*>(&data_[0]);
        else
            return NULL;
    }

    void setInt32Value(std::size_t pos, const int32_t& value)
    {
        if (pos >= data_.size())
        {
            ScopedWriteBoolLock lock(mutex_, true);
            if (pos >= data_.size())
                data_.resize(pos + 1, invalidValue_);
        }

        ScopedReadBoolLock lock(mutex_, true);
        data_[pos] = static_cast<T>(value);
        dirty_ = true;
    }
    void setFloatValue(std::size_t pos, const float& value)
    {
        if (pos >= data_.size())
        {
            ScopedWriteBoolLock lock(mutex_, true);
            if (pos >= data_.size())
                data_.resize(pos + 1, invalidValue_);
        }

        ScopedReadBoolLock lock(mutex_, true);
        data_[pos] = static_cast<T>(value);
        dirty_ = true;
    }
    void setInt64Value(std::size_t pos, const int64_t& value)
    {
        if (pos >= data_.size())
        {
            ScopedWriteBoolLock lock(mutex_, true);
            if (pos >= data_.size())
                data_.resize(pos + 1, invalidValue_);
        }

        ScopedReadBoolLock lock(mutex_, true);
        data_[pos] = static_cast<T>(value);
        dirty_ = true;
    }
    void setDoubleValue(std::size_t pos, const double& value)
    {
        if (pos >= data_.size())
        {
            ScopedWriteBoolLock lock(mutex_, true);
            if (pos >= data_.size())
                data_.resize(pos + 1, invalidValue_);
        }

        ScopedReadBoolLock lock(mutex_, true);
        data_[pos] = static_cast<T>(value);
        dirty_ = true;
    }
    bool setStringValue(std::size_t pos, const std::string& value)
    {
        if (pos >= data_.size())
        {
            ScopedWriteBoolLock lock(mutex_, true);
            if (pos >= data_.size())
                data_.resize(pos + 1, invalidValue_);
        }

        ScopedReadBoolLock lock(mutex_, true);
        try
        {
            data_[pos] = boost::lexical_cast<T>(value);
            dirty_ = true;
        }
        catch (const boost::bad_lexical_cast &)
        {
            return false;
        }
        return true;
    }

    void setValue(std::size_t pos, const T& value)
    {
        if (pos >= data_.size())
        {
            ScopedWriteBoolLock lock(mutex_, true);
            if (pos >= data_.size())
                data_.resize(pos + 1, invalidValue_);
        }

        ScopedReadBoolLock lock(mutex_, true);
        data_[pos] = value;
        dirty_ = true;
    }

    void copyValue(std::size_t from, std::size_t to)
    {
        ScopedWriteLock lock(mutex_);
        if (from >= data_.size() || data_[from] == invalidValue_)
            return;

        if (to >= data_.size())
            data_.resize(to + 1, invalidValue_);

        data_[to] = data_[from];
        dirty_ = true;
    }

    int compareValues(std::size_t lhs, std::size_t rhs, bool isLock) const
    {
        ScopedReadBoolLock lock(mutex_, isLock);
        const T& lv = data_[lhs];
        if (lv == invalidValue_) return -1;
        const T& rv = data_[rhs];
        if (rv == invalidValue_) return 1;
        if (lv < rv) return -1;
        if (lv > rv) return 1;
        return 0;
    }

    void clearValue(std::size_t pos)
    {
        ScopedWriteLock lock(mutex_);
        if (pos < data_.size())
        {
            data_[pos] = invalidValue_;
            dirty_ = true;
        }
    }

protected:
    void load_(std::istream& is)
    {
        ScopedWriteLock lock(mutex_);
        std::size_t len = 0;
        is.read((char*)&len, sizeof(len));
        data_.resize(len);
        is.read((char*)&data_[0], sizeof(T) * len);
    }

    void save_(std::ostream& os) const
    {
        ScopedReadLock lock(mutex_);
        std::size_t len = data_.size();
        os.write((const char*)&len, sizeof(len));
        os.write((const char*)&data_[0], sizeof(T) * len);
    }

protected:
    bool dirty_;
    T invalidValue_;
    std::string path_;
    std::vector<T> data_;
};

template <>
inline bool NumericPropertyTable<int64_t>::getStringValue(std::size_t pos, std::string& value, bool isLock) const
{
    ScopedReadBoolLock lock(mutex_, isLock);
    if (pos >= data_.size() || data_[pos] == invalidValue_)
        return false;

    using namespace boost::posix_time;
    if (type_ == DATETIME_PROPERTY_TYPE)
    {
        value = to_iso_string(from_time_t(data_[pos] - timezone));
    }
    else
    {
        value = boost::lexical_cast<std::string>(data_[pos]);
    }
    return true;
}

template <>
inline bool NumericPropertyTable<int8_t>::getStringValue(std::size_t pos, std::string& value, bool isLock) const
{
    ScopedReadBoolLock lock(mutex_, isLock);
    if (pos >= data_.size() || data_[pos] == invalidValue_)
        return false;
    value = boost::lexical_cast<std::string>(boost::numeric_cast<int32_t>(data_[pos]));
    return true;
}

template <>
inline bool NumericPropertyTable<float>::getStringValue(std::size_t pos, std::string& value, bool isLock) const
{
    ScopedReadBoolLock lock(mutex_, isLock);
    if (pos >= data_.size() || data_[pos] == invalidValue_)
        return false;
    char buf[32];
    modp_dtoa((double)data_[pos], buf, kPrecisionFloat);
    value.assign(buf);
    return true;
}

template <>
inline bool NumericPropertyTable<double>::getStringValue(std::size_t pos, std::string& value, bool isLock) const
{
    ScopedReadBoolLock lock(mutex_, isLock);
    if (pos >= data_.size() || data_[pos] == invalidValue_)
        return false;
    char buf[32];
    modp_dtoa((double)data_[pos], buf, kPrecisionDouble);
    value.assign(buf);
    return true;
}

template <>
inline bool NumericPropertyTable<int8_t>::setStringValue(std::size_t pos, const std::string& value)
{
    if (pos >= data_.size())
    {
        ScopedWriteBoolLock lock(mutex_, true);
        if (pos >= data_.size())
            data_.resize(pos + 1, invalidValue_);
    }

    try
    {
        data_[pos] = boost::numeric_cast<int8_t>(boost::lexical_cast<int32_t>(value));
        dirty_ = true;
    }
    catch (const boost::bad_lexical_cast &)
    {
        return false;
    }
    return true;
}


}

#endif
