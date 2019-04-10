#ifndef SEEDING_SPACEPOINT_HPP
#define SEEDING_SPACEPOINT_HPP

#include <algorithm>
#include <cmath>
#include <ostream>
#include <vector>

namespace tricktrack {

struct Vector3D {

  Vector3D(double x, double y, double z) : m_x(x), m_y(y), m_z(z) {}

  double m_x;
  double m_y;
  double m_z;

  double x() const { return m_x; }
  double y() const { return m_y; }
  double z() const { return m_z; }
};

/// @brief 3D space point for track seeding.
///
/// @tparam Identifier  A value-like type that is used to link back to the
///                     original hits.
template <typename Identifier>
class SpacePoint {
public:
  SpacePoint(double x, double y, double z, Identifier id) : m_position(x, y, z), m_identifier(id) {}

  const Vector3D& position() const { return m_position; }
  double x() const { return m_position.x(); }
  double y() const { return m_position.y(); }
  double z() const { return m_position.z(); }
  double rho() const { return std::sqrt(std::pow(m_position.x(), 2) + std::pow(m_position.y(), 2)); }
  double phi() const { return std::atan2(m_position.y(), m_position.x()); }
  const Identifier& identifier() const { return m_identifier; }

private:
  Vector3D m_position;
  Identifier m_identifier;
};

template <typename Identifier>
inline std::ostream& operator<<(std::ostream& os, const SpacePoint<Identifier>& point) {
  os << "rho=" << point.rho() << " phi=" << point.phi() << " z=" << point.z();
  return os;
}

}  // namespace tricktrack

#endif  // SEEDING_SPACEPOINT_HPP
