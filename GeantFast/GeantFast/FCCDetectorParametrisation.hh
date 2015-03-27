#ifndef FCC_DETECTOR_PARAMETRSIATION_H
#define FCC_DETECTOR_PARAMETRSIATION_H

#include "globals.hh"

/**
	@brief     Definition of detector resolution and efficiency.
   @details   A simple class used to provide the detector resolution and efficiency (dependent on the detector, parametrisation type and particle momentum).
   @author    Anna Zaborowska
*/

class FCCDetectorParametrisation
{
public:
   /**
      A default constructor.
    */
   FCCDetectorParametrisation();
   ~FCCDetectorParametrisation();
   /**
      A parametrisation type (CMS, ATLAS, ALEPH or AtlFast-like).
    */
   enum Parametrisation {eCMS, eATLAS, eALEPH, eATLFAST};
   /**
      A detector type (tracking detector, electromagnetic calorimeter or hadronic calorimeter).
    */
   enum Detector {eTRACKER, eEMCAL, eHCAL};
   /**
      Gets the resolution of a detector for a given particle.
      @param aDetector A detector type.
      @param aParamterisation A parametrisation type.
      @param aMomentum A particle momentum.
    */
   G4double GetResolution(Detector aDetector, Parametrisation aParamterisation, G4double aMomentum);
   /**
      Gets the efficiency of a detector for a given particle.
      @param aDetector A detector type.
      @param aParamterisation A parametrisation type.
      @param aMomentum A particle momentum.
    */
   G4double GetEfficiency(Detector aDetector, Parametrisation aParamterisation, G4double aMomentum);

};

#endif
