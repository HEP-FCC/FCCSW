#ifndef SIMG4COMPONENTS_G4PRIMARIESFROMEDMTOOL_H
#define SIMG4COMPONENTS_G4PRIMARIESFROMEDMTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "FWCore/DataHandle.h"

#include "SimG4Interface/IG4EventProviderTool.h"

#include "G4VUserPrimaryGeneratorAction.hh"

// Forward declarations
// datamodel
namespace fcc {
class MCParticleCollection;
}

/** @class G4PrimariesFromEdmTool G4PrimariesFromEdmTool.h "G4PrimariesFromEdmTool.h"
*
*  Tool to translate an EDM MCParticleCollection into a G4Event
*
*  @author A. Zaborowska, J. Lingemann, A. Dell'Acqua
*  @date   2016-01-11
*/
class G4PrimariesFromEdmTool : public GaudiTool, virtual public IG4EventProviderTool {
public:
  /// Standard constructor
  G4PrimariesFromEdmTool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~G4PrimariesFromEdmTool();

  StatusCode initialize() final;

  /// Translates the input (fcc::MCParticleCollection) into a G4Event
  /// @returns G4Event with primaries generated from MCParticleCollection (ownership is transferred to the caller)
  virtual G4Event* g4Event() final;

private:
  /// Handle for the EDM MC particles to be read
  DataHandle<fcc::MCParticleCollection> m_genParticles;
};

#endif
