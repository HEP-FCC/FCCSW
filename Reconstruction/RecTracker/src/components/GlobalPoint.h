#ifndef RECTRACKER_GLOBALPOINT_H
#define RECTRACKER_GLOBALPOINT_H

#include <ctgmath>
#include <cinttypes>
#include <iostream>



/// Adapted from CMSSW via https://github.com/JuantAldea/FastFitters
class GlobalPoint {
public:
  GlobalPoint() : m_v{0, 0, 0} {}

  GlobalPoint(float x, float y, float z) : m_v{x, y, z} {}

  GlobalPoint(const GlobalPoint& o) : m_v{o[0], o[1], o[2]} {}

  float x() const { return m_v[0]; }

  float y() const { return m_v[1]; }

  float z() const { return m_v[2]; }

  float& operator[](const uint8_t index) { return m_v[index]; }

  const float& operator[](const uint8_t index) const { return const_cast<GlobalPoint&>(*this)[index]; }

  GlobalPoint operator-(const GlobalPoint& o) const { return GlobalPoint(m_v[0] - o[0], m_v[1] - o[1], m_v[2] - o[2]); }

  GlobalPoint operator+(const GlobalPoint& o) const { return GlobalPoint(m_v[0] + o[0], m_v[1] + o[1], m_v[2] + o[2]); }

  GlobalPoint operator*(const float scalar) const { return GlobalPoint(m_v[0] * scalar, m_v[1] * scalar, m_v[2] * scalar); }

private:
  float m_v[3];
};

std::ostream& operator<<(std::ostream& os, const GlobalPoint& obj) {
  os << "(" << obj[0] << "," << obj[1] << "," << obj[2] << ")";
  return os;
}

GlobalPoint normalize(const GlobalPoint& a);
float dot(const GlobalPoint& a, const GlobalPoint& b);
float length(const GlobalPoint& a);
float length2(const GlobalPoint& a);
float perp2(const GlobalPoint& p);
float perp(const GlobalPoint& p);

// magnitude of the transverse component
float perp(const GlobalPoint& p) { return sqrt(perp2(p)); }

// squared magnitude of the transverse component
inline float perp2(const GlobalPoint& p) { return p[0] * p[0] + p[1] * p[1]; }

inline float length2(const GlobalPoint& a) { return dot(a, a); }

inline float length(const GlobalPoint& a) { return sqrt(length2(a)); }

inline GlobalPoint normalize(const GlobalPoint& a) {
  float magnitude = length(a);
  return GlobalPoint(a[0] / magnitude, a[1] / magnitude, a[2] / magnitude);
}

inline float dot(const GlobalPoint& a, const GlobalPoint& b) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; }

#endif
