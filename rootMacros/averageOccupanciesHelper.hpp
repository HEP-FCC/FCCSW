#ifndef ROOT_AVERAGEOCCUPANCIESHELPER_HPP
#define ROOT_AVERAGEOCCUPANCIESHELPER_HPP

#include "ActsStyle.hpp"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile2D.h"
#include "TString.h"
#include "TTree.h"

// number of layers
static const int nLayers_barrel = 12;
static const int nLayers_ec = 20;
// masks for layers
const long long int mask = 0xf;
const long long int layerMaskBarrel = 0x1f0;
const long long int posNegMask = 0x10;
const long long int layerMaskEC = 0x3e0;

struct ModuleInformation {
  /// moduleID
  unsigned long long moduleID = -1;
  /// Occupancy (average)
  float occupancy = 0;
  /// Occupancy max
  float occupancyMax = std::numeric_limits<float>::min();
  /// Occupancy min
  float occupancyMin = std::numeric_limits<float>::max();
  /// Number of tracks per cluster (average)
  int nTracks;
  /// cluster size x (average)
  int sizeX = 0;
  /// cluster size y (average)
  int sizeY = 0;
  /// Surface position x
  float x = 0;
  /// Surface position y
  float y = 0;
  /// Surface position z
  float z = 0;
};

struct LayerInformation {
  /// cluster sizes in r/phi direction
  TProfile* clusterSizes0_eta_h = nullptr;
  /// cluster sizes in z direction
  TProfile* clusterSizes1_eta_h = nullptr;
  /// cluster sizes total
  TProfile* clusterSizes_eta_h = nullptr;
  /// cluster sizes in r/phi direction
  TProfile* clusterSizes0_theta_h = nullptr;
  /// cluster sizes in z direction
  TProfile* clusterSizes1_theta_h = nullptr;
  /// cluster sizes total
  TProfile* clusterSizes_theta_h = nullptr;
  /// cluster sizes in r/phi direction
  TProfile* clusterSizes0_phi_h = nullptr;
  /// cluster sizes in z direction
  TProfile* clusterSizes1_phi_h = nullptr;
  /// cluster sizes total
  TProfile* clusterSizes_phi_h = nullptr;
  /// occupancy of each event for this layer
  std::vector<float> occupancies;
  /// Occupancy (average)
  double occupancy = 0;
  /// Occupancy max
  float occupancyMax = std::numeric_limits<float>::min();
  /// Occupancy min
  float occupancyMin = std::numeric_limits<float>::max();
  /// cluster sizes in X of each event for this layer
  std::vector<float> sizesX;
  /// cluster sizes in Y of each event for this layer
  std::vector<float> sizesY;
  /// cluster size x (average)
  float sizeX = 0;
  /// cluster size y (average)
  float sizeY = 0;
  /// number of cells of cluster (average)
  double nCells = 0;
  // number of surfaces hit
  int nSurfaces = 0;
  // Merged cluster rate
  double mergedClusterRate = 0;
  /// number of clusters per layer
  double nClusters = 0;
  /// data rate
  double dataRate = 0;

  void add(const LayerInformation& layerInfo) {
    occupancies.push_back(layerInfo.occupancy);
    occupancy += layerInfo.occupancy;
    sizesX.push_back(layerInfo.sizeX);
    sizesY.push_back(layerInfo.sizeY);
    sizeX += layerInfo.sizeX;
    sizeY += layerInfo.sizeY;
    nCells += layerInfo.nCells;
    mergedClusterRate += layerInfo.mergedClusterRate;
    dataRate += layerInfo.dataRate;
    if (layerInfo.occupancyMax > occupancyMax) occupancyMax = layerInfo.occupancyMax;
    if (layerInfo.occupancyMin < occupancyMin) occupancyMin = layerInfo.occupancyMin;
  };

  void standardDev(float& stdOccupancies, float& stdSizesX, float& stdSizesY) {
    double sumOccupancy = 0;
    double sumSizeX = 0;
    double sumSizeY = 0;

    if (occupancies.size() != sizesX.size() || occupancies.size() != sizesY.size()) {
      throw std::runtime_error(
          "standardDev::Number of entries in 'occupancies', 'sizesX' and 'sizesY' different. This should not happen!");
    }

    for (int i = 0; i < occupancies.size(); i++) {
      // occupancy
      double diffOccupancy = occupancies.at(i) - occupancy;
      sumOccupancy += (diffOccupancy * diffOccupancy);
      // size x
      double diffSizeX = sizesX.at(i) - sizeX;
      sumSizeX += (diffSizeX * diffSizeX);
      // size y
      double diffSizeY = sizesY.at(i) - sizeY;
      sumSizeY += (diffSizeY * diffSizeY);
    }
    if (occupancies.size() > 1) {
      sumOccupancy /= (occupancies.size() - 1);
      sumSizeX /= (occupancies.size() - 1);
      sumSizeY /= (occupancies.size() - 1);
    }
    stdOccupancies = sqrt(sumOccupancy);
    stdSizesX = sqrt(sumSizeX);
    stdSizesY = sqrt(sumSizeY);
  };

  /// Number of tracks per cluster (average)
  /*  int nTracks;
    /// cluster size x (average)
    int sizeX = 0;
    /// cluster size y (average)
    int sizeY = 0;
    /// Surface position x
    float x = 0;
    /// Surface position y
    float y = 0;
    /// Surface position z
    float z = 0;*/
};

struct EventInformation {

  /// Layer information
  std::vector<LayerInformation> layerInfo_barrel;
  std::vector<LayerInformation> layerInfo_ePos;
  std::vector<LayerInformation> layerInfo_eNeg;
  /// default constructor
  EventInformation() : layerInfo_barrel(nLayers_barrel), layerInfo_ePos(nLayers_ec), layerInfo_eNeg(nLayers_ec) {}
};

#endif
