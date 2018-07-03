#ifndef DIGITIZATION_CLUSTERWRITER_H
#define DIGITIZATION_CLUSTERWRITER_H
#include "ACTS/Surfaces/Surface.hpp"
#include "ACTS/Utilities/BinUtility.hpp"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
#include "RecTracker/IClusterWriter.h"
#include "TFile.h"
#include "TTree.h"

/** @class ClusterWriter
 *
 *	This writer writes out cluster information for each cluster.
 *
 *  @author Julia Hrdinka
 *  @date   2018-04
 */

namespace sim {
class FCCPlanarCluster;
}

class ClusterWriter : public GaudiTool, virtual public IClusterWriter {

public:
  /// Constructor
  ClusterWriter(const std::string& type, const std::string& name, const IInterface* parent);
  /// Destructor
  ~ClusterWriter() = default;
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
  /// volume identifier
  int m_volumeID;
  /// layer identifier
  int m_layerID;
  /// surface identifier
  int m_surfaceID;
  /// global x
  float m_x;
  /// global y
  float m_y;
  /// global z
  float m_z;
  /// The total number of channels of this module
  int m_nChannels;
  /// The number of channels turned on of this module and event
  int m_nChannelsOn;
  /// Number of tracks per cluster
  int m_nTracksPerCluster;
  /// The cluster energy
  int m_energy;
  /// cluster size in x
  int m_size_x;
  /// cluster size in y
  int m_size_y;
};

#endif  // DIGITIZATION_CLUSTERWRITER_H
