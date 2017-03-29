#ifndef TESTGEOMETRY_GDMLDETECTORCONSTRUCTION_H
#define TESTGEOMETRY_GDMLDETECTORCONSTRUCTION_H

// Geant
#include "G4GDMLParser.hh"
#include "G4VUserDetectorConstruction.hh"

// Gaudi
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

/** @class GdmlDetectorConstruction TestGeometry/TestGeometry/GdmlDetectorConstruction.h GdmlDetectorConstruction.h
 *
 *  Detector construction from the GDML file. It includes simple sensitive detector construction for test purposes.
 *
 *  @author Anna Zaborowska
 */

namespace test {
class GdmlDetectorConstruction : public G4VUserDetectorConstruction {
public:
  /**  Constructor.
   *   @param[in] aFileName Name of the GDML file with the detector description.
   */
  explicit GdmlDetectorConstruction(const std::string& aFileName);
  virtual ~GdmlDetectorConstruction();
  /**  Create volumes using the GDML parser.
   *   @return World wolume.
   */
  virtual G4VPhysicalVolume* Construct() override;
  /**  Create sensitive detectors. Simple sensitive detector is created.
   *   See Geant examples/extended/persistency/gdml/G04 for further details.
   */
  virtual void ConstructSDandField() override;

private:
  /// Message Service
  ServiceHandle<IMessageSvc> m_msgSvc;
  /// Message Stream
  MsgStream m_log;
  /// Parser of the GDML file
  G4GDMLParser m_parser;
  /// name of the GDML file
  std::string m_fileName;
};
}

#endif /* TESTGEOMETRY_GDMLDETECTORCONSTRUCTION_H */
