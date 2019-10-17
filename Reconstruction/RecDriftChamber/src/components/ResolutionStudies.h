#ifndef RECDRIFTCHAMBER_RESOLUTIONSTUDIES_H
#define RECDRIFTCHAMBER_RESOLUTIONSTUDIES_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"

#include "datamodel/TrackHit.h"
#include "datamodel/TrackHitCollection.h"

// FCCSW
#include "FWCore/DataHandle.h"


class IGeoSvc;
/*
namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
}
*/

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
  /// Names of the fields for which neighbours are found
  /*
  std::vector<std::string> m_fieldNames;
  /// Minimal and maximal values of the fields for which neighbours are found
  std::vector<std::pair<int, int>> m_fieldExtremes;

  DataHandle<fcc::PositionedTrackHitCollection> m_positionedHits{"positionedHits", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::TrackHitCollection> m_mergedTrackHits{"mergedHits", Gaudi::DataHandle::Writer, this};

  Gaudi::Property<std::string> m_outFileName{this, "outFileName", "", "Output filename"};
  Gaudi::Property<double> m_EdepCut{this, "EdepCut", 0.1, "Edep Cut"};
  Gaudi::Property<double> m_DCACut{this, "DCACut", 0.1, "DCACut Cut"};
  */

};

#endif /* RECDRIFTCHAMBER_RESOLUTIONSTUDIES_H */
