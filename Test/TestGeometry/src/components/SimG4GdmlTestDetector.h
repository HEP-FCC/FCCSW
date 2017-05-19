#ifndef TESTGEOMETRY_G4GDMLTESTDETECTOR_H
#define TESTGEOMETRY_G4GDMLTESTDETECTOR_H

// Gaudi
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "SimG4Interface/ISimG4DetectorConstruction.h"

// Geant4
#include "G4GDMLParser.hh"

/** @class SimG4GdmlTestDetector TestGeometry/src/components/SimG4GdmlTestDetector.h SimG4GdmlTestDetector.h
 *
 *  Detector construction tool using the GDML file. It creates the detector as defined in
 * test::GdmlDetectorConstruction.
 *  GDML file name needs to be specified in job options file (\b'gdml').
 *
 *  @author Anna Zaborowska
 */

class SimG4GdmlTestDetector : public AlgTool, virtual public ISimG4DetectorConstruction {
public:
  explicit SimG4GdmlTestDetector(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SimG4GdmlTestDetector();
  /**  Initialize.
   *   @return status code
   */
  StatusCode initialize() override;
  /**  Finalize.
   *   @return status code
   */
  StatusCode finalize() override;
  /** Get the initilization of the geometry.
   *  @return pointer to G4VUserDetectorConstruction
   */
  G4VUserDetectorConstruction* detectorConstruction() override;

private:
  /// name of the GDML file
  Gaudi::Property<std::string> m_gdmlFile{this, "gdml", "", "name of the GDML file"};
};

#endif /* TESTGEOMETRY_G4GDMLTESTDETECTOR_H */
