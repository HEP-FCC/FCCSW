#ifndef SIMG4COMPONENTS_G4GDMLDETECTOR_H
#define SIMG4COMPONENTS_G4GDMLDETECTOR_H

// Gaudi
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "SimG4Interface/ISimG4DetectorConstruction.h"

// Geant4
#include "G4GDMLParser.hh"

/** @class SimG4GdmlDetector SimG4Components/src/SimG4GdmlDetector.h SimG4GdmlDetector.h
 *
 *  Detector construction tool using the GDML file.
 *  GDML file name needs to be specified in job options file (\b'gdml').
 *
 *  @author Anna Zaborowska
 */

class SimG4GdmlDetector : public AlgTool, virtual public ISimG4DetectorConstruction {
public:
  explicit SimG4GdmlDetector(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SimG4GdmlDetector();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /** Get the initilization of the geometry.
   *  @return pointer to G4VUserDetectorConstruction (ownership is transferred to the caller)
   */
  virtual G4VUserDetectorConstruction* detectorConstruction();

private:
  /// name of the GDML file
  Gaudi::Property<std::string> m_gdmlFile{this, "gdml", "", "name of the GDML file"};
};

#endif /* SIMG4COMPONENTS_G4GDMLDETECTOR_H */
