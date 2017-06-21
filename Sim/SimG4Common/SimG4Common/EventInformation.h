#ifndef SIMG4COMMON_EVENTINFORMATION_H
#define SIMG4COMMON_EVENTINFORMATION_H

#include "G4VUserEventInformation.hh"

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

  /// Set the collections to store the secondaries from the current event, ownership is _not_ transfered
  void setCollections(fcc::GenVertexCollection* genVertices, fcc::MCParticleCollection* mcParticles);
  /// Add a particle to be tracked in the EDM collections
  void addParticle(const G4Track* aSecondary);

  void Print() const {};

private:
  /// Pointer to the vertex collection, ownership should be handled in a algorithm / tool
  fcc::GenVertexCollection* m_genVertices;
  /// Pointer to the particle collection, ownership should be handled in a algorithm / tool
  fcc::MCParticleCollection* m_mcParticles;
  /// Map to get the edm end vertex id from a Geant4 unique particle ID
  std::map<size_t, size_t> m_g4IdToEndVertexMap;
};
}
#endif /* define SIMG4COMMON_EVENTINFORMATION_H */
