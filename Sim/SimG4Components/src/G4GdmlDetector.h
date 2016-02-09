#ifndef SIMG4COMPONENTS_G4GDMLDETECTOR_H
#define SIMG4COMPONENTS_G4GDMLDETECTOR_H

// Gaudi
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "SimG4Interface/IG4DetectorConstruction.h"

// Geant4
#include "G4GDMLParser.hh"

/** @class G4GdmlDetector SimG4Components/src/G4GdmlDetector.h G4GdmlDetector.h
 *
 *  Detector construction tool using the GDML file.
 *  GDML file name needs to be specified in job options file (\b'gdml').
 *
 *  @author Anna Zaborowska
 */

class G4GdmlDetector: public AlgTool, virtual public IG4DetectorConstruction {
public:
  explicit G4GdmlDetector(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~G4GdmlDetector();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /** Get the initilization of the geometry.
   *  @return pointer to G4VUserDetectorConstruction
   */
  virtual G4VUserDetectorConstruction* getDetectorConstruction();
private:
  /// name of the GDML file
  std::string m_gdmlFile;
};

#endif /* SIMG4COMPONENTS_G4GDMLDETECTOR_H */
