#include "foo.h"

namespace qyuqyu {

namespace {

constexpr uint64 uvnan{0x7FF8000000000001};
constexpr uint64 uvinf{0x7FF0000000000000};
constexpr uint64 uvneginf{0xFFF0000000000000};
constexpr uint64 uvone{0x3FF0000000000000};
constexpr uint64 mask{0x7FF};
constexpr uint64 shift{52};
constexpr uint64 bias{1023};
constexpr uint64 signMask{1ULL << 63ULL};
constexpr uint64 fracMask{(1ULL << shift) - 1ULL};

uint64 bits(const float64 f) {
  auto *p = &f;
  return *reinterpret_cast<const uint64 *>(reinterpret_cast<const void *>(p));
}

float64 fromBits(const uint64 b) {
  auto *p = &b;
  return *reinterpret_cast<const float64 *>(reinterpret_cast<const void *>(p));
}

float64 inf(const int sign) {
  uint64 v{};
  if (sign >= 0) {
    v = uvinf;
  } else {
    v = uvneginf;
  }
  return fromBits(v);
}

float64 nan() { return fromBits(uvnan); }

float64 isInf(const float64 f, const int sign) {
  return ((sign >= 0) && (f > MaxFloat64)) ||
         ((sign <= 0) && (f < -MaxFloat64));
}

bool isNan(const float64 f) { return f != f; }

void modf(const float64 f, float64 *num, float64 *frac) {
  if (f < 1.0) {
    if (f < 0.0) {
      modf(-f, num, frac);
      *num = -(*num);
      *frac = -(*frac);
      return;
    } else if (f == 0.0) {
      *num = f;
      *frac = f;
      return;
    }
    *num = 0.0;
    *frac = f;
    return;
  }
  auto x = bits(f);
  auto e = ((x >> shift) & mask) - bias;
  if (e < shift) {
    uint64 q{(1ULL << (shift - e)) - 1ULL};
    x = x & ~q;
  }
  *num = fromBits(x);
  *frac = f - (*num);
  return;
}

} // namespace

float64 abs(const float64 f) { return fromBits(bits(f) & ~signMask); }

float64 sqrt(const float64 f) {
  if ((f == 0.0) || isNan(f) || isInf(f, 1)) {
    return f;
  } else if (f < 0.0) {
    return nan();
  }
  auto ix = bits(f);
  auto exp = static_cast<int>((ix >> shift) & mask);
  if (exp == 0) {
    while ((ix & (1ULL << shift)) == 0) {
      ix <<= 1;
      exp -= 1;
    }
    exp += 1;
  }
  exp -= bias;
  ix &= ~(mask << shift);
  ix |= 1ULL << shift;
  if ((exp & 1) == 1) {
    ix <<= 1;
  }
  exp >>= 1;
  ix <<= 1;
  uint64 q{}, s{};
  uint64 r{1ULL << (shift + 1ULL)};
  while (r != 0) {
    auto t = s + r;
    if (t <= ix) {
      s = t + r;
      ix -= t;
      q += r;
    }
    ix <<= 1;
    r >>= 1;
  }
  if (ix != 0) {
    q += q & 1;
  }
  ix = (q >> 1) + (static_cast<uint64>(exp - 1 + bias) << shift);
  return fromBits(ix);
}

float64 max(const float64 x, const float64 y) {
  if (isInf(x, 1) || isInf(y, 1)) {
    return inf(1);
  } else if (isNan(x) || isNan(y)) {
    return nan();
  } else if ((x == 0.0) && (x == y)) {
    return y;
  }
  if (x > y) {
    return x;
  }
  return y;
}

float64 min(const float64 x, const float64 y) {
  if (isInf(x, -1) || isInf(y, -1)) {
    return inf(-1);
  } else if (isNan(x) || isNan(y)) {
    return nan();
  } else if ((x == 0.0) && (x == y)) {
    return y;
  }
  if (x < y) {
    return x;
  }
  return y;
}

float64 floor(float64 f) {
  if ((f == 0.0) || isNan(f) || isInf(f, 0)) {
    return f;
  }
  if (f < 0.0) {
    float64 d{}, frac{};
    modf(-f, &d, &frac);
    if (frac != 0.0) {
      d += 1.0;
    }
    return -d;
  }
  float64 d{}, _{};
  modf(f, &d, &_);
  return d;
}

float64 ceil(float64 f) { return -floor(-f); }

float64 trunc(float64 f) {
  if ((f == 0.0) || isNan(f) || isInf(f, 0)) {
    return f;
  }
  float64 d{}, _{};
  modf(f, &d, &_);
  return d;
}

float64 round(float64 f) {
  auto b = bits(f);
  auto e = (b >> shift) & mask;
  if (e < bias) {
    b &= signMask;
    if (e == (bias - 1)) {
      b |= uvone;
    }
  } else if (e < (bias + shift)) {
    constexpr uint64 half{1ULL << (shift - 1ULL)};
    e -= bias;
    b += half >> e;
    b &= ~(fracMask >> e);
  }
  return fromBits(b);
}

// int add(int a, int b) { return a + b; }
} // namespace qyuqyu
