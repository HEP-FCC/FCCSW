#ifndef SIMG4COMPONENTS_G4SINGLEPARTICLEGENERATORTOOL_H
#define SIMG4COMPONENTS_G4SINGLEPARTICLEGENERATORTOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimG4Interface/ISimG4EventProviderTool.h"

// Geant4
#include "G4SystemOfUnits.hh"

// Forward declarations
// Geant4
class G4Event;
class G4PrimaryVertex;
class G4PrimaryParticle;
// datamodel
namespace fcc {
class MCParticleCollection;
class GenVertexCollection;
}

/** @class SimG4SingleParticleGeneratorTool SimG4SingleParticleGeneratorTool.h "SimG4SingleParticleGeneratorTool.h"
*
*  Tool that generates single particles with parameters set via options file.
*
*  @author Andrea Dell'Acqua, J. Lingemann
*  @date   2014-10-01
*/

class SimG4SingleParticleGeneratorTool : public GaudiTool, virtual public ISimG4EventProviderTool {
public:
  /// Standard constructor
  SimG4SingleParticleGeneratorTool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual StatusCode initialize() final;

  /// Destructor
  virtual ~SimG4SingleParticleGeneratorTool();

  /// Generates primaries using the parameters set via options file, uses CLHEP:RandFlat random number generator
  /// @returns G4Event with primaries generated through G4ParticleGun (ownership is transferred to the caller)
  virtual G4Event* g4Event() final;

private:
  /// Saves primary vertex and particle to FCC EDM (called if property saveEDM is set to true)
  StatusCode saveToEdm(const G4PrimaryVertex*, const G4PrimaryParticle*);
  /// Minimum energy of the particles generated, set with energyMin
  Gaudi::Property<double> m_energyMin{this, "energyMin", 1 * CLHEP::GeV, "Minimum energy of generated particles"};
  /// Maximum energy of the particles generated, set with energyMax
  Gaudi::Property<double> m_energyMax{this, "energyMax", 1 * CLHEP::TeV, "Maximum energy of generated particles"};
  /// Minimum eta of the particles generated, set with etaMin
  Gaudi::Property<double> m_etaMin{this, "etaMin", -5., "Minimum eta of generated particles"};
  /// Maximum eta of the particles generated, set with etaMax
  Gaudi::Property<double> m_etaMax{this, "etaMax", 5, "Maximum eta of generated particles"};
  /// Minimum phi of the particles generated, set with phiMin
  Gaudi::Property<double> m_phiMin{this, "phiMin", 0., "Minimum phi of generated particles"};
  /// Maximum phi of the particles generated, set with phiMax
  Gaudi::Property<double> m_phiMax{this, "phiMax", 2 * M_PI, "Maximum phi of generated particles"};
  /// x position of the vertex associated with the particles generated, set with vertexX
  Gaudi::Property<double> m_vertexX{this, "vertexX", 0};
  /// y position of the vertex associated with the particles generated, set with vertexY
  Gaudi::Property<double> m_vertexY{this, "vertexY", 0};
  /// z position of the vertex associated with the particles generated, set with vertexZ
  Gaudi::Property<double> m_vertexZ{this, "vertexZ", 0};
  /// Name of the generated particle, set with particleName
  Gaudi::Property<std::string> m_particleName{this, "particleName", "geantino", "Name of the generated particles"};
  /// Flag whether to save primary particle to EDM, set with saveEdm
  Gaudi::Property<bool> m_saveEdm{this, "saveEdm", false};
  /// Handle for the genparticles to be written
  DataHandle<fcc::MCParticleCollection> m_genParticlesHandle{"GenParticles", Gaudi::DataHandle::Writer, this};
  /// Handle for the genvertices to be written
  DataHandle<fcc::GenVertexCollection> m_genVerticesHandle{"GenVertices", Gaudi::DataHandle::Writer, this};
};

#endif
