#ifndef DIGITIZATION_MODULECLUSTERWRITER_H
#define DIGITIZATION_MODULECLUSTERWRITER_H
#include "ACTS/Surfaces/Surface.hpp"
#include "ACTS/Utilities/BinUtility.hpp"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
#include "RecTracker/IClusterWriter.h"
#include "TFile.h"
#include "TTree.h"

/** @class ModuleClusterWriter
 *
 *  This writer writes out cluster information per module. It internally uses a module cache.
 *
 *  @author Julia Hrdinka
 *  @date   2018-07
 */

namespace sim {
class FCCPlanarCluster;
}

class ModuleClusterWriter : public GaudiTool, virtual public IClusterWriter {

  ///@struct ModuleCache
  ///@brief internal helper struct
  /// This struct is a module cache, to accumulate all relevant information per module.
  struct ModuleCache {
    bool wasFilled = false;
    int _nChannelsOn = 0;
    std::vector<float> _x;
    std::vector<float> _y;
    std::vector<float> _z;
    std::vector<short int> _nTracksPerCluster;
    std::vector<short int> _sizeX;
    std::vector<short int> _sizeY;
    std::vector<float> _energy;

    // default constructor
    ModuleCache() {
      _x.reserve(1000000);
      _y.reserve(1000000);
      _z.reserve(1000000);
      _nTracksPerCluster.reserve(1000000);
      _sizeX.reserve(1000000);
      _sizeY.reserve(1000000);
      _energy.reserve(1000000);
    }

    // simple constructor
    ModuleCache(const int& nChannelsOn, const float& x, const float& y, const float& z,
                const unsigned short& nTracksPerCluster, const unsigned short& sizeX, const unsigned short& sizeY,
                const float& energy)
        : _nChannelsOn(nChannelsOn) {
      _x.push_back(x);
      _y.push_back(y);
      _z.push_back(z);
      _nTracksPerCluster.push_back(nTracksPerCluster);
      _sizeX.push_back(sizeX);
      _sizeY.push_back(sizeY);
      _energy.push_back(energy);
    }

    void update(const int& nChannelsOn, const float& x, const float& y, const float& z,
                const unsigned short& nTracksPerCluster, const unsigned short& sizeX, const unsigned short& sizeY,
                const float& energy) {
      _nChannelsOn += nChannelsOn;
      _x.push_back(x);
      _y.push_back(y);
      _z.push_back(z);
      _nTracksPerCluster.push_back(nTracksPerCluster);
      _sizeX.push_back(sizeX);
      _sizeY.push_back(sizeY);
      _energy.push_back(energy);
    }

    void clear() {
      _nChannelsOn = 0;
      _x.clear();
      _y.clear();
      _z.clear();
      _nTracksPerCluster.clear();
      _sizeX.clear();
      _sizeY.clear();
      _energy.clear();
    }
  };

public:
  /// Constructor
  ModuleClusterWriter(const std::string& type, const std::string& name, const IInterface* parent);
  /// Destructor
  ~ModuleClusterWriter() = default;
  /// Gaudi interface initialization method
  virtual StatusCode initialize() final;
  /// Gaudi interface finalize method
  virtual StatusCode finalize() final;
  /// Gaudi interface execute method
  virtual StatusCode write(const sim::FCCPlanarCluster& cluster, int eventNr = 0) override final;

private:
  /// name of the output file
  Gaudi::Property<std::string> m_filename{this, "filename", "PlanarClusters.root", "file name to save the clusters to"};
  /// name of the output tree
  Gaudi::Property<std::string> m_treename{this, "treename", "clusters", "tree name to save the clusters to"};

  /// the output file
  TFile* m_outputFile;
  /// the output tree
  TTree* m_outputTree;
  /// the event number of
  int m_eventNr;
  /// module identifier
  ///@todo can we change that to 32? is already surface ID
  long long int m_moduleID;
  /// The total number of channels of this module
  int m_nChannels;
  /// The number of channels turned on of this module and event
  int m_nChannelsOn;
  /// global x of module
  float m_sX;
  /// global y of module
  float m_sY;
  /// global z of module
  float m_sZ;
  /// global x of cluster
  std::vector<float> m_x;
  /// global y of cluster
  std::vector<float> m_y;
  /// global z of cluster
  std::vector<float> m_z;
  /// Number of tracks per cluster
  std::vector<short int> m_nTracksPerCluster;
  /// cluster size in x
  std::vector<short int> m_sizeX;
  /// cluster size in y
  std::vector<short int> m_sizeY;
  /// The cluster energy
  std::vector<float> m_energy;
  /// current surface cache
  ModuleCache m_moduleCache;

  /// update module cache and parameters
  void newModule(int eventNr, const long long int& moduleID, int nChannels, int nChannelsOn, float sX, float sY,
                 float sZ, float x, float y, float z, unsigned short nTracksPerCluster, unsigned short sizeX,
                 unsigned short sizeY, float energy) {
    // 1) first write out data of previous module
    // fill the tree if it is not the first module
    if (m_moduleID >= 0) {
      m_nChannelsOn = m_moduleCache._nChannelsOn;
      m_x = m_moduleCache._x;
      m_y = m_moduleCache._y;
      m_z = m_moduleCache._z;
      m_nTracksPerCluster = m_moduleCache._nTracksPerCluster;
      m_sizeX = m_moduleCache._sizeX;
      m_sizeY = m_moduleCache._sizeY;
      m_energy = m_moduleCache._energy;

      long long int mask = 0xf;
      long long int systemID = (m_moduleID & mask);
      m_outputTree->Fill();
    }
    // 2) reset everything and fill in new data
    // first update parameters per module
    m_eventNr = eventNr;
    m_moduleID = moduleID;
    long long int mask = 0xf;
    long long int systemID = (m_moduleID & mask);
    m_nChannels = nChannels;
    m_sX = sX;
    m_sY = sY;
    m_sZ = sZ;
    // update aggregated parameters
    m_moduleCache.clear();
    m_moduleCache.update(nChannelsOn, x, y, z, nTracksPerCluster, sizeX, sizeY, energy);
  };
};

#endif  // DIGITIZATION_MODULECLUSTERWRITER_H
