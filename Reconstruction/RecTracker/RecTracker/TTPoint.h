// TODO: this file is part of tricktrack and should be removed once it becomes available as an external
#ifndef SEEDING_TTPOINT_HPP
#define SEEDING_TTPOINT_HPP

#include <algorithm>
#include <cmath>
#include <ostream>
#include <vector>
#include <array>

namespace tricktrack {


class TTPoint {
public:
  TTPoint(double r, double phi, double z, double t, unsigned int id) : m_coordinates{r, phi, z, t}, m_identifier(id) {}

  double x() const { return m_coordinates[0] * std::cos(m_coordinates[1]); }
  double y() const { return m_coordinates[0] * std::sin(m_coordinates[1]); }
  double z() const { return m_coordinates[2]; }
  double t() const { return m_coordinates[3]; }
  double rho() const { return m_coordinates[0];}
  double phi() const { return m_coordinates[1];}
  const unsigned int identifier() const { return m_identifier; }

  double& operator[](unsigned int const i) { return m_coordinates[i]; }

  double const& operator[](unsigned int const i) const { return m_coordinates[i]; }
  unsigned int getId() const { return m_identifier; }

private:
  std::array<double, 4> m_coordinates;
  unsigned int m_identifier;
};


}  // namespace tricktrack

#endif  // SEEDING_TTPOINT_HPP
