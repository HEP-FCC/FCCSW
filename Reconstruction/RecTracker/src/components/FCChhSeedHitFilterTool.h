#ifndef RECTRACKER_FCCHHSEEDHITFILTERTOOL_H
#define RECTRACKER_FCCHHSEEDHITFILTERTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "RecInterface/IHitFilterTool.h"
#include "RecInterface/ILayerGraphTool.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

//#include "tricktrack/SpacePoint.h"
//#include "tricktrack/TrackingRegion.h"
//#include "tricktrack/HitChainMaker.h"

#include <map>

/** @class FCChhSeedHitFilterTool
 *  Checks if a hit belongs to a set of configurable seeding layers without using the dd4hep volumemanager
 */
class FCChhSeedHitFilterTool : public GaudiTool, virtual public IHitFilterTool {
public:
  FCChhSeedHitFilterTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~FCChhSeedHitFilterTool() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  virtual bool filter(fcc::TrackHit hit) override final;
  virtual void setIds(unsigned int systemId, unsigned int layerId) override final;
  /// in this implementation the more specific setIds is used
  virtual void setState(std::vector<double> params) override final {};
private:

  unsigned int m_layerId = 0;
  unsigned int m_systemId = 0;

};

#endif /* RECTRACKER_FCCHHSEEDHITFILTERTOOL_H */
