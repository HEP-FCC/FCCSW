
#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"


#include "DD4hep/Detector.h"
#include "DD4hep/Volumes.h"
#include "DetSegmentation/GridDriftChamber.h"

#include <cmath>
#include <random>

#include "ResolutionStudies.h"
#include "DetCommon/DetUtils.h"



DECLARE_ALGORITHM_FACTORY(ResolutionStudies)



ResolutionStudies::ResolutionStudies(const std::string& name, ISvcLocator* svcLoc) : 
GaudiAlgorithm(name, svcLoc)
{
  //  declareProperty("mergedHits", m_mergedTrackHits, "Merged Tracker hits (Output)");
  //  declareProperty("positionedHits", m_positionedHits, "Positioned hits (Input)");
}

StatusCode ResolutionStudies::initialize() {
  info() << "ResolutionStudies initialize" << endmsg;

  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the "
               "configuration."
            << endmsg;
    return StatusCode::FAILURE;
  }

  // check if readouts exist

  if (m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_readoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }


  // retrieve  segmentation
  m_segmentation = dynamic_cast<dd4hep::DDSegmentation::GridDriftChamber*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (m_segmentation == nullptr) {
    error() << "There is no drift chamber segmentation." << endmsg;
    return StatusCode::FAILURE;
  }

  // Take readout bitfield decoder from GeoSvc
  
  m_decoder =
      dynamic_cast<dd4hep::DDSegmentation::BitFieldCoder*>(m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder());
  // check if decoder contains "layer"
  std::vector<std::string> fields;
  for (uint itField = 0; itField < m_decoder->size(); itField++) {
    fields.push_back((*m_decoder)[itField].name());
  }
  auto iter = std::find(fields.begin(), fields.end(), "layer");
  if (iter == fields.end()) {
    error() << "Readout does not contain field: 'layer'" << endmsg;
    //    addLayerRestriction = false;
  }

  std::cout << "HERE" << std::endl;
  auto allwires = m_segmentation->returnAllWires();
  
  for (const auto& element : allwires)
    {
      auto wires = element.second;
      for (unsigned i = 0; i < wires.size(); ++i)
	{
	  auto middle = wires[i].first;
	  auto direction = wires[i].second;

	  middle.Print();
	}
    }

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) 
    {
      return sc;
    }

  return sc;
}

StatusCode ResolutionStudies::execute() {
  





  return StatusCode::SUCCESS;
}

StatusCode ResolutionStudies::finalize() {

  // m_tree->Write();
  // file->Close();

  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
