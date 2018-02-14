#ifndef SIMG4COMMON_EVENTINFORMATION_H
#define SIMG4COMMON_EVENTINFORMATION_H

#include "G4VUserEventInformation.hh"

#include <iostream>
#include <map>

class G4Track;
namespace fcc {
class GenVertexCollection;
class MCParticleCollection;
}

/** @class sim::EventInformation SimG4Common/SimG4Common/EventInformation.h EventInformation.h
 *
 * Additional event information.
 *
 * Currently holds the particle history in form of edm particles and vertices
 *
 * @author J. Lingemann
 */

namespace sim {
class EventInformation : public G4VUserEventInformation {
public:
  /// Default constructor
  EventInformation();
  /// Destructor
  virtual ~EventInformation() = default;
  /// set external pointers to point at the particle and vertex collections
  void setCollections(fcc::GenVertexCollection*& aGenVertexCollection, fcc::MCParticleCollection*& aMcParticleCollection);
  /// Add a particle to be tracked in the EDM collections
  void addParticle(const G4Track* aSecondary);

  void Print() const {};

private:
  /// Pointer to the vertex collection, ownership is intended to be transfered to SaveTool
  fcc::GenVertexCollection* m_genVertices;
  /// Pointer to the particle collection, ownership is intended to be transfered to SaveTool
  fcc::MCParticleCollection* m_mcParticles;
  /// Map to get the edm end vertex id from a Geant4 unique particle ID
  std::map<size_t, size_t> m_g4IdToEndVertexMap;
};
}
#endif /* define SIMG4COMMON_EVENTINFORMATION_H */
