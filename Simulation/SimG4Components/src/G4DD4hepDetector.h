#ifndef SIMG4COMPONENTS_G4DD4HEPDETECTOR_H
#define SIMG4COMPONENTS_G4DD4HEPDETECTOR_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimG4Interface/IG4DetectorConstruction.h"
class IGeoSvc;

/** @class G4DD4hepDetector SimG4Components/src/G4DD4hepDetector.h G4DD4hepDetector.h
 *
 *  Detector construction tool using the DD4hep.
 *  Retrieves the geometry build by the implementation of IGeoSvc.
 *
 *  @author Anna Zaborowska
 */

class G4DD4hepDetector: public GaudiTool, virtual public IG4DetectorConstruction {
  public:
  explicit G4DD4hepDetector(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~G4DD4hepDetector();
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
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
};

#endif /* SIMG4COMPONENTS_G4DD4HEPDETECTOR_H */
