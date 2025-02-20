#pragma once

#include <cassert>
#include <cfloat>
#include <cmath>

#include "zeus/CVector3f.hpp"
#include "zeus/Global.hpp"

#if ZE_ATHENA_TYPES
#include <athena/IStreamReader.hpp>
#endif

namespace zeus {
class CColor;

class CVector4f {
public:
  zeus::simd<float> mSimd;

  constexpr CVector4f() : mSimd(0.f) {}

  template <typename T>
  constexpr CVector4f(const simd<T>& s) : mSimd(s) {}

#if ZE_ATHENA_TYPES

  constexpr CVector4f(const atVec4f& vec) : mSimd(vec.simd) {}

  operator atVec4f&() { return *reinterpret_cast<atVec4f*>(this); }

  operator const atVec4f&() const { return *reinterpret_cast<const atVec4f*>(this); }

  void readBig(athena::io::IStreamReader& input) {
    simd_floats f;
    f[0] = input.readFloatBig();
    f[1] = input.readFloatBig();
    f[2] = input.readFloatBig();
    f[3] = input.readFloatBig();
    mSimd.copy_from(f);
  }

#endif

  explicit constexpr CVector4f(float xyzw) : mSimd(xyzw) {}

  void assign(float x, float y, float z, float w) { mSimd = simd<float>(x, y, z, w); }

  constexpr CVector4f(float x, float y, float z, float w) : mSimd(x, y, z, w) {}

  constexpr CVector4f(const CColor& other);

  CVector4f(const CVector3f& other, float wIn = 1.f) : mSimd(other.mSimd) { mSimd[3] = wIn; }

  static CVector4f ToClip(const zeus::CVector3f& v, float w) { return CVector4f(v * w, w); }

  CVector3f toVec3f() const { return CVector3f(mSimd); }

  constexpr CVector4f& operator=(const CColor& other);

  bool operator==(const CVector4f& rhs) const {
    auto eq_mask = mSimd == rhs.mSimd;
    return eq_mask[0] && eq_mask[1] && eq_mask[2] && eq_mask[3];
  }

  bool operator!=(const CVector4f& rhs) const {
    auto eq_mask = mSimd != rhs.mSimd;
    return eq_mask[0] || eq_mask[1] || eq_mask[2] || eq_mask[3];
  }

  bool operator<(const CVector4f& rhs) const {
    auto eq_mask = mSimd < rhs.mSimd;
    return eq_mask[0] && eq_mask[1] && eq_mask[2] && eq_mask[3];
  }

  bool operator<=(const CVector4f& rhs) const {
    auto eq_mask = mSimd <= rhs.mSimd;
    return eq_mask[0] && eq_mask[1] && eq_mask[2] && eq_mask[3];
  }

  bool operator>(const CVector4f& rhs) const {
    auto eq_mask = mSimd > rhs.mSimd;
    return eq_mask[0] && eq_mask[1] && eq_mask[2] && eq_mask[3];
  }

  bool operator>=(const CVector4f& rhs) const {
    auto eq_mask = mSimd >= rhs.mSimd;
    return eq_mask[0] && eq_mask[1] && eq_mask[2] && eq_mask[3];
  }

  CVector4f operator+(const CVector4f& rhs) const { return mSimd + rhs.mSimd; }

  CVector4f operator-(const CVector4f& rhs) const { return mSimd - rhs.mSimd; }

  CVector4f operator-() const { return -mSimd; }

  CVector4f operator*(const CVector4f& rhs) const { return mSimd * rhs.mSimd; }

  CVector4f operator/(const CVector4f& rhs) const { return mSimd / rhs.mSimd; }

  CVector4f operator+(float val) const { return mSimd + zeus::simd<float>(val); }

  CVector4f operator-(float val) const { return mSimd - zeus::simd<float>(val); }

  CVector4f operator*(float val) const { return mSimd * zeus::simd<float>(val); }

  CVector4f operator/(float val) const {
    float ooval = 1.f / val;
    return mSimd * zeus::simd<float>(ooval);
  }

  const CVector4f& operator+=(const CVector4f& rhs) {
    mSimd += rhs.mSimd;
    return *this;
  }

  const CVector4f& operator-=(const CVector4f& rhs) {
    mSimd -= rhs.mSimd;
    return *this;
  }

  const CVector4f& operator*=(const CVector4f& rhs) {
    mSimd *= rhs.mSimd;
    return *this;
  }

  const CVector4f& operator/=(const CVector4f& rhs) {
    mSimd /= rhs.mSimd;
    return *this;
  }

  void normalize() {
    float mag = magnitude();
    mag = 1.f / mag;
    *this *= CVector4f(mag);
  }

  CVector4f normalized() const {
    float mag = magnitude();
    mag = 1.f / mag;
    return *this * mag;
  }

  float dot(const CVector4f& rhs) const { return mSimd.dot4(rhs.mSimd); }

  float magSquared() const { return mSimd.dot4(mSimd); }

  float magnitude() const { return std::sqrt(magSquared()); }

  void zeroOut() { mSimd = zeus::simd<float>(0.f); }

  void splat(float xyzw) { mSimd = zeus::simd<float>(xyzw); }

  static CVector4f lerp(const CVector4f& a, const CVector4f& b, float t) {
    return zeus::simd<float>(1.f - t) * a.mSimd + b.mSimd * zeus::simd<float>(t);
  }

  static CVector4f nlerp(const CVector4f& a, const CVector4f& b, float t) { return lerp(a, b, t).normalized(); }

  bool isNormalized() const { return std::fabs(1.f - magSquared()) < 0.01f; }

  bool canBeNormalized() const {
    if (std::isinf(x()) || std::isinf(y()) || std::isinf(z()) || std::isinf(w()))
      return false;
    return std::fabs(x()) >= FLT_EPSILON || std::fabs(y()) >= FLT_EPSILON || std::fabs(z()) >= FLT_EPSILON ||
           std::fabs(w()) >= FLT_EPSILON;
  }

  bool isEqu(const CVector4f& other, float epsilon = FLT_EPSILON) const {
    const CVector4f diffVec = other - *this;
    return (diffVec.x() <= epsilon && diffVec.y() <= epsilon && diffVec.z() <= epsilon && diffVec.w() <= epsilon);
  }

  zeus::simd<float>::reference operator[](size_t idx) {
    assert(idx < 4);
    return mSimd[idx];
  }

  float operator[](size_t idx) const {
    assert(idx < 4);
    return mSimd[idx];
  }

  float x() const { return mSimd[0]; }
  float y() const { return mSimd[1]; }
  float z() const { return mSimd[2]; }
  float w() const { return mSimd[3]; }

  simd<float>::reference x() { return mSimd[0]; }
  simd<float>::reference y() { return mSimd[1]; }
  simd<float>::reference z() { return mSimd[2]; }
  simd<float>::reference w() { return mSimd[3]; }
};
constexpr CVector4f skOne4f(1.f);
constexpr CVector4f skNegOne4f(-1.f);
constexpr CVector4f skZero4f(0.f);

inline CVector4f operator+(float lhs, const CVector4f& rhs) { return zeus::simd<float>(lhs) + rhs.mSimd; }

inline CVector4f operator-(float lhs, const CVector4f& rhs) { return zeus::simd<float>(lhs) - rhs.mSimd; }

inline CVector4f operator*(float lhs, const CVector4f& rhs) { return zeus::simd<float>(lhs) * rhs.mSimd; }

inline CVector4f operator/(float lhs, const CVector4f& rhs) { return zeus::simd<float>(lhs) / rhs.mSimd; }

} // namespace zeus
