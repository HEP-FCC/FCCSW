#ifndef DIGITIZATION_DIGITIZATIONUTILS_H
#define DIGITIZATION_DIGITIZATIONUTILS_H

#include "ACTS/Digitization/DigitizationModule.hpp"
#include "ACTS/Digitization/PlanarModuleCluster.hpp"
#include "ACTS/Digitization/Segmentation.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "TFile.h"
#include "TTree.h"
#include <stdexcept>
#include <string>

namespace digi {
// write out planar module clusters per event
void writePlanarClusters(std::vector<std::vector<Acts::PlanarModuleCluster>> clustersPerEvent) {
  // @todo make that parameters
  std::string filePath = "PlanarClusters.root";
  std::string fileMode = "RECREATE";
  std::string treeName = "clusters";

  std::mutex m_writeMutex;         ///< protect multi-threaded writes
  TFile* m_outputFile;             ///< the output file
  TTree* m_outputTree;             ///< the output tree
  int m_eventNr;                   ///< the event number of
  int m_volumeID;                  ///< volume identifier
  int m_layerID;                   ///< layer identifier
  int m_surfaceID;                 ///< surface identifier
  float m_x;                       ///< global x
  float m_y;                       ///< global y
  float m_z;                       ///< global z
  float m_lx;                      ///< local lx
  float m_ly;                      ///< local ly
  float m_cov_lx;                  ///< local covariance lx
  float m_cov_ly;                  ///< local covariance ly
  std::vector<int> m_cell_IDx;     ///< cell ID in lx
  std::vector<int> m_cell_IDy;     ///< cell ID in ly
  std::vector<float> m_cell_lx;    ///< local cell position x
  std::vector<float> m_cell_ly;    ///< local cell position y
  std::vector<float> m_cell_data;  ///< local cell position y

  // optional the truth position
  std::vector<float> m_t_gx;     ///< truth position global x
  std::vector<float> m_t_gy;     ///< truth position global y
  std::vector<float> m_t_gz;     ///< truth position global z
  std::vector<float> m_t_lx;     ///< truth position local x
  std::vector<float> m_t_ly;     ///< truth position local y
  std::vector<int> m_t_barcode;  ///< associated truth particle barcode

  // Setup ROOT I/O
  m_outputFile = TFile::Open(filePath.c_str(), fileMode.c_str());
  if (!m_outputFile) {
    throw std::ios_base::failure("Could not open '" + filePath);
  }
  m_outputFile->cd();
  m_outputTree = new TTree(treeName.c_str(), "TTree from RootPlanarClusterWriter");
  if (!m_outputTree) throw std::bad_alloc();

  // Set the branches
  m_outputTree->Branch("event_nr", &m_eventNr);
  m_outputTree->Branch("volumeID", &m_volumeID);
  m_outputTree->Branch("layerID", &m_layerID);
  m_outputTree->Branch("surfaceID", &m_surfaceID);
  m_outputTree->Branch("g_x", &m_x);
  m_outputTree->Branch("g_y", &m_y);
  m_outputTree->Branch("g_z", &m_z);
  m_outputTree->Branch("l_x", &m_lx);
  m_outputTree->Branch("l_y", &m_ly);
  m_outputTree->Branch("cov_l_x", &m_cov_lx);
  m_outputTree->Branch("cov_l_y", &m_cov_ly);
  m_outputTree->Branch("cell_ID_x", &m_cell_IDx);
  m_outputTree->Branch("cell_ID_y", &m_cell_IDy);
  m_outputTree->Branch("cell_l_x", &m_cell_lx);
  m_outputTree->Branch("cell_l_y", &m_cell_ly);
  m_outputTree->Branch("cell_data", &m_cell_data);
  // @todo write out truth
  /*  m_outputTree->Branch("truth_g_x", &m_t_gx);
    m_outputTree->Branch("truth_g_y", &m_t_gy);
    m_outputTree->Branch("truth_g_z", &m_t_gz);
    m_outputTree->Branch("truth_l_x", &m_t_lx);
    m_outputTree->Branch("truth_l_y", &m_t_ly);
    m_outputTree->Branch("truth_barcode", &m_t_barcode);*/

  // go through events and their clusters
  for (auto clusters = clustersPerEvent.begin(); clusters != clustersPerEvent.end(); clusters++) {
    // go through clusters of this surface
    for (auto& cluster : (*clusters)) {
      // set event number
      m_eventNr = std::distance(clustersPerEvent.begin(), clusters);
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
      m_lx = local.x();
      m_ly = local.y();
      m_cov_lx = 0.;  // @todo fill in
      m_cov_ly = 0.;  // @todo fill in
                      // get the cells and run through them
      const auto& cells = cluster.digitizationCells();
      auto detectorElement = clusterSurface.associatedDetectorElement();
      for (auto& cell : cells) {
        // cell identification
        m_cell_IDx.push_back(cell.channel0);
        m_cell_IDy.push_back(cell.channel1);
        m_cell_data.push_back(cell.data);
        // for more we need the digitization module
        if (detectorElement && detectorElement->digitizationModule()) {
          auto digitationModule = detectorElement->digitizationModule();
          const Acts::Segmentation& segmentation = digitationModule->segmentation();
          // get the cell positions
          auto cellLocalPosition = segmentation.cellPosition(cell);
          m_cell_lx.push_back(cellLocalPosition.x());
          m_cell_ly.push_back(cellLocalPosition.y());
        }
      }
      // get the truth parameters
      /*   for (auto& tvertex : cluster.truthVertices()) {
           auto& tposition = tvertex.position();
           for (auto& tparticle : tvertex.incomingParticles()) {
             // truth global position
             m_t_gx.push_back(tposition.x());
             m_t_gy.push_back(tposition.y());
             m_t_gz.push_back(tposition.z());
             // local position
             auto& tmomentum = tparticle.momentum();
             Acts::Vector2D lposition;
             clusterSurface.globalToLocal(tposition, tmomentum, lposition);
             m_t_lx.push_back(lposition.x());
             m_t_ly.push_back(lposition.y());
             m_t_barcode.push_back(tparticle.barcode());
           }
         }*/
      // fill the tree
      m_outputTree->Fill();
      // now reset
      m_cell_IDx.clear();
      m_cell_IDy.clear();
      m_cell_lx.clear();
      m_cell_ly.clear();
      m_cell_data.clear();
      m_t_gx.clear();
      m_t_gy.clear();
      m_t_gz.clear();
      m_t_lx.clear();
      m_t_ly.clear();
      m_t_barcode.clear();
    }  // clusters
  }    // event

  // write out all clusters
  m_outputFile->cd();
  m_outputTree->Write();
  m_outputFile->Close();
}
}

#endif  // DIGITIZATION_DIGITIZATIONUTILS_H
