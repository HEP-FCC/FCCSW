#include "SimG4Data/GeantTrackerHit.h"

GeantTrackerHit::GeantTrackerHit() {}

GeantTrackerHit::~GeantTrackerHit() {}

GeantTrackerHit::GeantTrackerHit(const GeantTrackerHit &right): G4VHit() {
  m_energyDeposit = right.m_energyDeposit;
  m_position = right.m_position;
}

const GeantTrackerHit& GeantTrackerHit::operator=(const GeantTrackerHit &right) {
  m_energyDeposit = right.m_energyDeposit;
  m_position = right.m_position;
  return *this;
}

int GeantTrackerHit::operator==(const GeantTrackerHit &right) const {
  return (this==&right) ? 1 : 0;
}

void GeantTrackerHit::SetEnergyDeposit(double aDeposit) {
  m_energyDeposit = aDeposit;
}
double GeantTrackerHit::GetEnergyDeposit() const {
  return m_energyDeposit;
}
void GeantTrackerHit::SetPosition(CLHEP::Hep3Vector aPosition) {
  m_position = aPosition;
}
CLHEP::Hep3Vector GeantTrackerHit::GetPosition() const {
  return m_position;
}
