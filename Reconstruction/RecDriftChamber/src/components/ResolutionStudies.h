#ifndef RECDRIFTCHAMBER_RESOLUTIONSTUDIES_H
#define RECDRIFTCHAMBER_RESOLUTIONSTUDIES_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"


// FCCSW
#include "FWCore/DataHandle.h"


class IGeoSvc;

class ResolutionStudies : public GaudiAlgorithm {
public:
  ResolutionStudies(const std::string& name, ISvcLocator* svcLoc);

  ~ResolutionStudies() = default;

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  

private:
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;

  dd4hep::DDSegmentation::GridDriftChamber* m_segmentation;
  dd4hep::DDSegmentation::BitFieldCoder* m_decoder;

  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "", "Name of the readout (hits collection) to save"};

};

#endif /* RECDRIFTCHAMBER_RESOLUTIONSTUDIES_H */
