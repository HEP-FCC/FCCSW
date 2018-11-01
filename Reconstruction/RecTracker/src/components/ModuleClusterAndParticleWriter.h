#ifndef DIGITIZATION_MODULECLUSTERANDPARTICLEWRITER_H
#define DIGITIZATION_MODULECLUSTERANDPARTICLEWRITER_H
#include "ACTS/Surfaces/Surface.hpp"
#include "ACTS/Utilities/BinUtility.hpp"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
#include "RecTracker/IClusterWriter.h"
#include "TFile.h"
#include "TMatrixD.h"
#include "TTree.h"

/** @class ModuleClusterAndParticleWriter
 *
 *  This writer writes out cluster information per module and the simulated particles information. It internally uses a
 * module cache.
 *
 *  @author Julia Hrdinka
 *  @date   2018-07
 */

namespace sim {
class FCCPlanarCluster;
}

class ModuleClusterAndParticleWriter : public GaudiTool, virtual public IClusterWriter {

  ///@struct ModuleCache
  ///@brief internal helper struct
  /// This struct is a module cache, to accumulate all relevant information per module.
  struct ModuleCache {
    bool wasFilled = false;
    int _nChannelsOn = 0;
    std::vector<float> _x;
    std::vector<float> _y;
    std::vector<float> _z;
    std::vector<std::vector<unsigned>> _trackIDsPerCluster;
    std::vector<short int> _nCells;
    std::vector<short int> _sizeX;
    std::vector<short int> _sizeY;
    std::vector<float> _energy;
    std::vector<float> _time;

    // default constructor
    ModuleCache() {
      _x.reserve(100);
      _y.reserve(100);
      _z.reserve(100);
      _trackIDsPerCluster.reserve(100);
      _nCells.reserve(100);
      _sizeX.reserve(100);
      _sizeY.reserve(100);
      _energy.reserve(100);
      _time.reserve(100);
    }

    // simple constructor
    ModuleCache(int nChannelsOn, float x, float y, float z, std::vector<unsigned> trackIDsPerCluster, short int sizeX,
                short int sizeY, float energy, float time)
        : _nChannelsOn(nChannelsOn) {
      _x.push_back(x);
      _y.push_back(y);
      _z.push_back(z);
      _trackIDsPerCluster.push_back(trackIDsPerCluster);
      _nCells.push_back(nChannelsOn);
      _sizeX.push_back(sizeX);
      _sizeY.push_back(sizeY);
      _energy.push_back(energy);
      _time.push_back(time);
    }

    void update(int nChannelsOn, float x, float y, float z, std::vector<unsigned> trackIDsPerCluster, short int sizeX,
                short int sizeY, float energy, float time) {
      _nChannelsOn += nChannelsOn;
      _x.push_back(x);
      _y.push_back(y);
      _z.push_back(z);
      _trackIDsPerCluster.push_back(trackIDsPerCluster);
      _nCells.push_back(nChannelsOn);
      _sizeX.push_back(sizeX);
      _sizeY.push_back(sizeY);
      _energy.push_back(energy);
      _time.push_back(time);
    }

    void clear() {
      _nChannelsOn = 0;
      _x.clear();
      _y.clear();
      _z.clear();
      _trackIDsPerCluster.clear();
      _nCells.clear();
      _sizeX.clear();
      _sizeY.clear();
      _energy.clear();
      _time.clear();
    }
  };

public:
  /// Constructor
  ModuleClusterAndParticleWriter(const std::string& type, const std::string& name, const IInterface* parent);
  /// Destructor
  ~ModuleClusterAndParticleWriter() = default;
  /// Gaudi interface initialization method
  virtual StatusCode initialize() final;
  /// Gaudi interface finalize method
  virtual StatusCode finalize() final;
  /// Gaudi interface execute method
  virtual StatusCode write(const sim::FCCPlanarCluster& cluster, int eventNr = 0) override final;
  /// particle track IDs
  //  virtual const std::set<unsigned>& trackIDs() const override final;
  TMatrixD translateToMatrix(const std::vector<std::vector<unsigned>>& trackIDs) const;

  std::vector<std::vector<unsigned>> translateMatrix(const TMatrixD& trackIDsMatrix) const;

private:
  /// name of the output file
  Gaudi::Property<std::string> m_filename{this, "filename", "PlanarClusters.root", "file name to save the clusters to"};
  /// name of the output tree
  Gaudi::Property<std::string> m_treename{this, "treename", "clusters", "tree name to save the clusters to"};
  /// name of the input file to write out the gen particles
  Gaudi::Property<std::string> m_inputFilename{this, "inputFileName", "",
                                               "input file name to write out simulated particles"};
  /// name of the input tree to write out the gen particles
  Gaudi::Property<std::string> m_inputTreename{this, "inputTreename", "events",
                                               "tree name of input simulated particle file"};
  /// name of the generated particles
  Gaudi::Property<std::string> m_genParticlesName{this, "genParticles", "genParticles",
                                                  "name of the generated particles branch"};
  /// name of the simulated particles
  Gaudi::Property<std::string> m_simParticlesName{this, "simParticles", "simParticles",
                                                  "name of the simulated particles branch"};

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
  /// The number of channels of this module of l0
  int m_nChannels_l0;
  /// The number of channels of this module of l1
  int m_nChannels_l1;
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
  /// TrackIDs per cluster
  TMatrixD m_trackIDsPerCluster;
  /// cluster
  std::vector<short int> m_nCells;
  /// cluster size in x
  std::vector<short int> m_sizeX;
  /// cluster size in y
  std::vector<short int> m_sizeY;
  /// The cluster energy
  std::vector<float> m_energy;
  /// The cluster time
  std::vector<float> m_time;
  /// current surface cache
  ModuleCache m_moduleCache;
  /// particle output
  /// the output file
  TFile* m_inputFile;
  /// the output tree
  TTree* m_inputTree;
  /// generated particles output tree
  TTree* m_genOutputTree;
  /// generated particle eta
  std::vector<float> m_gen_eta;
  /// generated particle phi
  std::vector<float> m_gen_phi;
  /// generated particle pt
  std::vector<float> m_gen_pt;
  /// generated particle energy
  std::vector<float> m_gen_energy;
  /// generated particle charge
  std::vector<int> m_gen_charge;
  /// generated particle status
  std::vector<int> m_gen_status;
  /// generated particle pdgID
  std::vector<int> m_gen_pdgid;
  /// generated particle vertex x
  std::vector<float> m_gen_vertexX;
  /// generated particle vertex y
  std::vector<float> m_gen_vertexY;
  /// generated particle vertex z
  std::vector<float> m_gen_vertexZ;
  /// simulated particles output tree
  TTree* m_simOutputTree;
  /// simulated particle eta
  std::vector<float> m_sim_eta;
  /// simulated particle phi
  std::vector<float> m_sim_phi;
  /// simulated particle pt
  std::vector<float> m_sim_pt;
  /// simulated particle energy
  std::vector<float> m_sim_energy;
  /// simulated particle charge
  std::vector<int> m_sim_charge;
  /// simulated bits
  std::vector<int> m_sim_bits;
  /// simulated particle status
  std::vector<int> m_sim_status;
  /// simulated particle pdgID
  std::vector<int> m_sim_pdgid;
  /// simulated particle vertex x
  std::vector<float> m_sim_vertexX;
  /// simulated particle vertex y
  std::vector<float> m_sim_vertexY;
  /// simulated particle vertex z
  std::vector<float> m_sim_vertexZ;
  /// debug
  std::set<int> trackIDsParticles;
  std::set<int> trackIDsClusters;

  /// update module cache and parameters
  void newModule(int eventNr, const long long int& moduleID, int nChannels, int nChannelsOn, int nChannels_l0,
                 int nChannels_l1, float sX, float sY, float sZ, float x, float y, float z,
                 std::vector<unsigned> trackIDsPerCluster, short int sizeX, short int sizeY, float energy, float time) {
    // 1) first write out data of previous module
    // fill the tree if it is not the first module
    if (m_moduleID >= 0) {
      m_nChannelsOn = m_moduleCache._nChannelsOn;
      m_x = m_moduleCache._x;
      m_y = m_moduleCache._y;
      m_z = m_moduleCache._z;
      auto trackIDsMatrix = translateToMatrix(m_moduleCache._trackIDsPerCluster);
      m_trackIDsPerCluster.ResizeTo(trackIDsMatrix.GetNrows(), trackIDsMatrix.GetNcols());
      m_trackIDsPerCluster = TMatrixD(trackIDsMatrix);
      m_nCells = m_moduleCache._nCells;
      m_sizeX = m_moduleCache._sizeX;
      m_sizeY = m_moduleCache._sizeY;
      m_energy = m_moduleCache._energy;
      m_time = m_moduleCache._time;
      m_outputTree->Fill();
    }
    // 2) reset everything and fill in new data
    // first update parameters per module
    m_eventNr = eventNr;
    m_moduleID = moduleID;
    m_nChannels = nChannels;
    m_nChannels_l0 = nChannels_l0;
    m_nChannels_l1 = nChannels_l1;
    m_sX = sX;
    m_sY = sY;
    m_sZ = sZ;
    // update aggregated parameters
    m_moduleCache.clear();
    m_moduleCache.update(nChannelsOn, x, y, z, trackIDsPerCluster, sizeX, sizeY, energy, time);
  };
};

inline TMatrixD
ModuleClusterAndParticleWriter::translateToMatrix(const std::vector<std::vector<unsigned>>& trackIDs) const {
  size_t maxTracksPerCluster = 30;
  TMatrixD trackIDsMatrix(trackIDs.size(), maxTracksPerCluster);
  for (size_t i = 0; i < trackIDs.size(); i++) {
    for (size_t j = 0; j < maxTracksPerCluster; j++) {
      if (j < trackIDs[i].size()) {
        trackIDsMatrix[i][j] = trackIDs[i][j];
      } else {
        trackIDsMatrix[i][j] = 0;
      }
    }
  }
  return trackIDsMatrix;
}

inline std::vector<std::vector<unsigned>>
ModuleClusterAndParticleWriter::translateMatrix(const TMatrixD& trackIDsMatrix) const {
  std::vector<std::vector<unsigned>> trackIDs;
  trackIDs.reserve(100);
  for (size_t i = 0; i < trackIDsMatrix.GetNrows(); i++) {
    std::vector<unsigned> trackIDsPerCluster;
    for (size_t j = 0; j < trackIDsMatrix.GetNcols(); j++) {
      unsigned trackID = trackIDsMatrix[i][j];
      if (trackID) trackIDsPerCluster.push_back(trackID);
    }
    trackIDs.push_back(trackIDsPerCluster);
  }
  return trackIDs;
}

#endif  // DIGITIZATION_MODULECLUSTERANDPARTICLEWRITER_H
