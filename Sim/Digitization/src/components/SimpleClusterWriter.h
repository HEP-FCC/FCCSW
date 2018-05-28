#ifndef DIGITIZATION_SIMPLECLUSTERWRITER_H
#define DIGITIZATION_SIMPLECLUSTERWRITER_H

#include "ACTS/Surfaces/Surface.hpp"
#include "ACTS/Utilities/BinUtility.hpp"
#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "TFile.h"
#include "TTree.h"

/** @class SimpleClusterWriter
 *
 *
 *  @author Julia Hrdinka
 *  @date   2017-04
 */

namespace sim {
class FCCPlanarCluster;
}

class SimpleClusterWriter : public GaudiAlgorithm {

public:
  /// Constructor
  SimpleClusterWriter(const std::string& name, ISvcLocator* svcLoc);
  /// Destructor
  ~SimpleClusterWriter() = default;
  /// Gaudi interface initialization method
  virtual StatusCode initialize() final;
  /// Gaudi interface finalize method
  virtual StatusCode finalize() final;
  /// Gaudi interface execute method
  virtual StatusCode execute() final;

private:
  /// the collection of input planar clusters
  DataHandle<std::vector<sim::FCCPlanarCluster>> m_planarClusterHandle{"planarClusters", Gaudi::DataHandle::Reader,
                                                                       this};
  /// name of the output file
  Gaudi::Property<std::string> m_filename{this, "filename", "PlanarClusters.root", "file name to save the clusters to"};
  /// name of the output tree
  Gaudi::Property<std::string> m_treename{this, "treename", "clusters", "tree name to save the clusters to"};

  TFile* m_outputFile;  ///< the output file
  TTree* m_outputTree;  ///< the output tree
  int m_eventNr;        ///< the event number of
  int m_volumeID;       ///< volume identifier
  int m_layerID;        ///< layer identifier
  int m_surfaceID;      ///< surface identifier
  float m_x;            ///< global x
  float m_y;            ///< global y
  float m_z;            ///< global z
  float m_lx;           ///< local x
  float m_ly;           ///< localy;

  int m_nChannels;    ///< The total number of channels of this module
  int m_nChannelsOn;  ///< The number of channels turned on of this module and event
                      /// float m_surfX;                  ///< global x of the surface center
                      /// float m_surfY;                  ///< global y of the surface center
                      //  float m_surfZ;                  ///< global z of the surface center
  int m_nTracksPerCluster;
  float m_energyPerCluster;
  int m_size_x;                  ///< cluster size in x
  int m_size_y;                  ///< cluster size in x
  long long int m_detElementID;  ///< unique identifier of the detector element
};

#endif  // DIGITIZATION_SIMPLECLUSTERWRITER_H
