#ifndef __QYUQYU_H__
#define __QYUQYU_H__
#include <cfloat>
namespace qyuqyu {
using uint64 = unsigned long long;
using float64 = long double;

constexpr float64 MaxFloat64{LDBL_MAX};

/// .
float64 abs(const float64 f);

/// .
float64 max(const float64 x, const float64 y);

/// .
float64 min(const float64 x, const float64 y);

/// .
float64 sqrt(const float64 f);

/// .
float64 floor(const float64 f);

/// .
float64 ceil(const float64 f);

/// .
float64 trunc(const float64 f);

/// .
float64 round(const float64 f);

} // namespace qyuqyu
#endif