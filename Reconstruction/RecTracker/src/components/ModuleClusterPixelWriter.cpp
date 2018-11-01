#include "ModuleClusterPixelWriter.h"

#include "ACTS/Digitization/DigitizationModule.hpp"
#include "ACTS/Digitization/PlanarModuleCluster.hpp"
#include "ACTS/Digitization/Segmentation.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Surfaces/RectangleBounds.hpp"
#include "ACTS/Surfaces/TrapezoidBounds.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "RecTracker/FCCDigitizationCell.h"
#include <cstring>
#include <string>

DECLARE_COMPONENT(ModuleClusterPixelWriter)

ModuleClusterPixelWriter::ModuleClusterPixelWriter(const std::string& type, const std::string& nam,
                                                   const IInterface* parent)
    : GaudiTool(type, nam, parent), m_moduleID(-1), m_nChannels(0), m_nChannelsOn(0), m_moduleCache() {}

StatusCode ModuleClusterPixelWriter::initialize() {
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
  m_outputTree = new TTree(m_treename.value().c_str(), "TTree from ModuleClusterPixelWriter");
  if (!m_outputTree) {
    error() << "Could not create TTree " << m_treename << endmsg;
    return StatusCode::FAILURE;
  }
  m_outputTree->SetDirectory(0);
  // Set the branches
  m_outputTree->Branch("event_nr", &m_eventNr);
  m_outputTree->Branch("moduleID", &m_moduleID);
  m_outputTree->Branch("nChannels", &m_nChannels);
  m_outputTree->Branch("nChannelsOn", &m_nChannelsOn);
  m_outputTree->Branch("nChannels_l0", &m_nChannels_l0);
  m_outputTree->Branch("nChannels_l1", &m_nChannels_l1);
  m_outputTree->Branch("modHalfY", &m_modHalfY);
  m_outputTree->Branch("modHalfX", &m_modHalfX);
  m_outputTree->Branch("s_x", &m_sX);
  m_outputTree->Branch("s_y", &m_sY);
  m_outputTree->Branch("s_z", &m_sZ);
  m_outputTree->Branch("p_x", &m_px);
  m_outputTree->Branch("p_y", &m_py);
  m_outputTree->Branch("g_x", &m_x);
  m_outputTree->Branch("g_y", &m_y);
  m_outputTree->Branch("g_z", &m_z);
  m_outputTree->Branch("tracksPerCluster", &m_tracksPerCluster);
  m_outputTree->Branch("nCells", &m_nCells);
  m_outputTree->Branch("size_x", &m_sizeX);
  m_outputTree->Branch("size_y", &m_sizeY);
  m_outputTree->Branch("energy", &m_energy);
  m_outputTree->Branch("time", &m_time);

  return StatusCode::SUCCESS;
}

StatusCode ModuleClusterPixelWriter::write(const sim::FCCPlanarCluster& cluster, int eventNr) {

  // access cluster surface
  auto& clusterSurface = cluster.referenceSurface();
  // surface center
  const Acts::Vector3D& surfaceCenter = clusterSurface.center();
  // local cluster information: position
  auto parameters = cluster.parameters();
  Acts::Vector2D local(parameters[Acts::ParDef::eLOC_0], parameters[Acts::ParDef::eLOC_1]);
  /// prepare for calculating the
  Acts::Vector3D pos(0, 0, 0);
  // transform local into global position information
  clusterSurface.localToGlobal(local, Acts::Vector3D(0., 0., 0.), pos);
  // the associated detector element
  auto detectorElement = clusterSurface.associatedDetectorElement();
  auto digitationModule = detectorElement->digitizationModule();
  const Acts::Segmentation& segmentation = digitationModule->segmentation();
  // the cells
  const auto& cells = cluster.digitizationCells();
  /// cell IDs in lx and ly
  std::vector<int> cell_IDx;
  std::vector<int> cell_IDy;

  std::vector<float> px;
  std::vector<float> py;
  for (auto& cell : cells) {
    // cell identification
    cell_IDx.push_back(cell.channel0);
    cell_IDy.push_back(cell.channel1);
    // cell position
    // get the cell positions
    auto cellLocalPosition = segmentation.cellPosition(cell);
    px.push_back(cellLocalPosition.x());
    py.push_back(cellLocalPosition.y());
  }
  // fill the cluster size
  short int sizeX =
      (*std::max_element(cell_IDx.begin(), cell_IDx.end())) - (*std::min_element(cell_IDx.begin(), cell_IDx.end())) + 1;
  short int sizeY =
      (*std::max_element(cell_IDy.begin(), cell_IDy.end())) - (*std::min_element(cell_IDy.begin(), cell_IDy.end())) + 1;

  long long int moduleID = detectorElement->identify();

  float modHalfX = 0.;
  float modHalfY = 0.;

  const Acts::RectangleBounds* recModuleBounds = dynamic_cast<const Acts::RectangleBounds*>(&(clusterSurface.bounds()));
  const Acts::TrapezoidBounds* trapModuleBounds =
      dynamic_cast<const Acts::TrapezoidBounds*>(&(clusterSurface.bounds()));
  if ((!recModuleBounds) && (!trapModuleBounds)) {
    error() << "Given SurfaceBounds are not planar - not implemented for "
               "other bounds yet! "
            << endmsg;
  } else if (recModuleBounds) {
    modHalfX = recModuleBounds->halflengthX();
    modHalfY = recModuleBounds->halflengthY();
  } else {
    modHalfX = trapModuleBounds->maxHalflengthX();
    modHalfY = trapModuleBounds->halflengthY();
  }

  // first check if we are still on the same surface
  if (moduleID == m_moduleID) {
    // we are still on the same surface - update
    m_moduleCache.update(px, py, cells.size(), pos.x(), pos.y(), pos.z(), cluster.trackIDs.size(), sizeX, sizeY,
                         cluster.energy, cluster.time);
  } else {
    // update module cache and parameters
    newModule(eventNr, moduleID, segmentation.binUtility().bins(), cells.size(), segmentation.binUtility().bins(0),
              segmentation.binUtility().bins(1), surfaceCenter.x(), surfaceCenter.y(), surfaceCenter.z(), pos.x(),
              pos.y(), pos.z(), cluster.trackIDs.size(), sizeX, sizeY, cluster.energy, cluster.time, px, py, modHalfY,
              modHalfX);
  }

  return StatusCode::SUCCESS;
}

StatusCode ModuleClusterPixelWriter::finalize() {
  if (GaudiTool::finalize().isFailure()) {
    return StatusCode::FAILURE;
  }

  // write out all clusters
  m_outputFile->cd();
  m_outputTree->Write();
  m_outputFile->Close();

  return StatusCode::SUCCESS;
}
