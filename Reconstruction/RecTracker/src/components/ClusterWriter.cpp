#include "ClusterWriter.h"
#include "ACTS/Digitization/DigitizationModule.hpp"
#include "ACTS/Digitization/PlanarModuleCluster.hpp"
#include "ACTS/Digitization/Segmentation.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "RecTracker/FCCDigitizationCell.h"
#include <cstring>
#include <string>

DECLARE_COMPONENT(ClusterWriter)

ClusterWriter::ClusterWriter(const std::string& type, const std::string& nam, const IInterface* parent)
    : GaudiTool(type, nam, parent) {}

StatusCode ClusterWriter::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }

  // Setup ROOT I/O
  m_outputFile = TFile::Open(m_filename.value().c_str(), "RECREATE");
  if (!m_outputFile) {
    error() << "Could not open " << m_filename << endmsg;
    return StatusCode::FAILURE;
  }
  m_outputFile->cd();
  m_outputTree = new TTree(m_treename.value().c_str(), "TTree from TrackClusterWriter");
  if (!m_outputTree) {
    error() << "Could not create TTree " << m_treename << endmsg;
    return StatusCode::FAILURE;
  }
  m_outputTree->SetDirectory(0);
  // Set the branches
  m_outputTree->Branch("event_nr", &m_eventNr);
  m_outputTree->Branch("volumeID", &m_volumeID);
  m_outputTree->Branch("layerID", &m_layerID);
  m_outputTree->Branch("surfaceID", &m_surfaceID);
  m_outputTree->Branch("g_x", &m_x);
  m_outputTree->Branch("g_y", &m_y);
  m_outputTree->Branch("g_z", &m_z);
  m_outputTree->Branch("nChannels", &m_nChannels);
  m_outputTree->Branch("nChannelsOn", &m_nChannelsOn);
  m_outputTree->Branch("nTracksPerCluster", &m_nTracksPerCluster);
  m_outputTree->Branch("energy", &m_energy);
  m_outputTree->Branch("size_x", &m_size_x);
  m_outputTree->Branch("size_y", &m_size_y);

  return StatusCode::SUCCESS;
}

StatusCode ClusterWriter::write(const sim::FCCPlanarCluster& cluster, int eventNr) {
  const auto& cells = cluster.digitizationCells();

  // set event number
  m_eventNr = eventNr;
  // the cluster surface
  auto& clusterSurface = cluster.referenceSurface();
  // set geometry IDs
  m_volumeID = clusterSurface.geoID().value(Acts::GeometryID::volume_mask);
  m_layerID = clusterSurface.geoID().value(Acts::GeometryID::layer_mask);
  m_surfaceID = clusterSurface.geoID().value(Acts::GeometryID::sensitive_mask);
  // local cluster information: position, @todo coveraiance
  auto parameters = cluster.parameters();
  Acts::Vector2D local(parameters[Acts::ParDef::eLOC_0], parameters[Acts::ParDef::eLOC_1]);
  /// prepare for calculating the
  Acts::Vector3D pos(0, 0, 0);
  Acts::Vector3D mom(1, 1, 1);
  // transform local into global position information
  clusterSurface.localToGlobal(local, mom, pos);
  // set position
  m_x = pos.x();
  m_y = pos.y();
  m_z = pos.z();
  m_nTracksPerCluster = cluster.trackIDs.size();
  m_energy = cluster.energy;

  // get the corresponding binUtility of the layer
  auto surfaces = clusterSurface.associatedLayer()->surfaceArray()->arrayObjects();

  std::vector<int> cell_IDx;  ///< cell ID in lx
  std::vector<int> cell_IDy;  ///< cell ID in ly

  auto detectorElement = clusterSurface.associatedDetectorElement();
  if (detectorElement && detectorElement->digitizationModule()) {
    auto digitationModule = detectorElement->digitizationModule();
    const Acts::Segmentation& segmentation = digitationModule->segmentation();
    // write out the total number of channels of this surface
    m_nChannels = segmentation.binUtility().bins();
    m_nChannelsOn = cells.size();

    for (auto& cell : cells) {
      // cell identification
      cell_IDx.push_back(cell.channel0);
      cell_IDy.push_back(cell.channel1);
    }
  }
  // fill the cluster size
  m_size_x =
      (*std::max_element(cell_IDx.begin(), cell_IDx.end())) - (*std::min_element(cell_IDx.begin(), cell_IDx.end())) + 1;
  m_size_y =
      (*std::max_element(cell_IDy.begin(), cell_IDy.end())) - (*std::min_element(cell_IDy.begin(), cell_IDy.end())) + 1;

  // fill the tree
  m_outputTree->Fill();
  return StatusCode::SUCCESS;
}

StatusCode ClusterWriter::finalize() {
  if (GaudiTool::finalize().isFailure()) {
    return StatusCode::FAILURE;
  }

  // write out all clusters
  m_outputFile->cd();
  m_outputTree->Write();
  m_outputFile->Close();

  return StatusCode::SUCCESS;
}
