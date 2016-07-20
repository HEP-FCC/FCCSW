#ifndef SIMG4COMPONENTS_G4SINGLEPARTICLEGENERATORTOOL_H
#define SIMG4COMPONENTS_G4SINGLEPARTICLEGENERATORTOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimG4Interface/ISimG4EventProviderTool.h"
#include "FWCore/DataHandle.h"

// Forward declarations
// Geant4
class G4Event;
class G4PrimaryVertex;
class G4PrimaryParticle;
//datamodel
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
  double m_energyMin;          ///< Minimum energy of the particles generated, set with energyMin
  double m_energyMax;          ///< Maximum energy of the particles generated, set with energyMax
  double m_etaMin;             ///< Minimum eta of the particles generated, set with etaMin
  double m_etaMax;             ///< Maximum eta of the particles generated, set with etaMax
  double m_phiMin;             ///< Minimum phi of the particles generated, set with phiMin
  double m_phiMax;             ///< Maximum phi of the particles generated, set with phiMax
  double m_vertexX;            ///< x position of the vertex associated with the particles generated, set with vertexX
  double m_vertexY;            ///< y position of the vertex associated with the particles generated, set with vertexY
  double m_vertexZ;            ///< z position of the vertex associated with the particles generated, set with vertexZ
  std::string m_particleName;  ///< Name of the generated particle, set with particleName
  bool m_saveEdm;              ///< Flag whether to save primary particle to EDM, set with saveEdm
  /// Handle for the genparticles to be written
  DataHandle<fcc::MCParticleCollection> m_genParticlesHandle;
  /// Handle for the genvertices to be written
  DataHandle<fcc::GenVertexCollection> m_genVerticesHandle;
};

#endif
